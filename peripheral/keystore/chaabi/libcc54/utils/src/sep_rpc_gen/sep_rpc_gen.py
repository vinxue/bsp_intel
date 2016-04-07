#!/usr/bin/python3

# (c) Copyright 2011-2012 Discretix Technologies Ltd.
# This file is licensed under the terms provided in the file
# libcc54/LICENSE in this directory or a parent directory

# Generator for SeP RPC wrappers

# Flow overview
# 0. Get command line arguments
# 1. Parse XML file into tree object
# 2. Scan XML tree object: foreach function: save function return code + for each parameter: Add attributes to input, output or i/o parameres list
# 3. H file generation:
# 	- File preamble: (Generated file + license, etc.)
# 	- Define API ID constant macro
# 	- Foreach function define index
# 	- For each function generate message structure:
# 	  1. Common public part
# 	  2. DMA references
# 	  3. input parameters
# 	  4. i/o parameters
# 	  5. output parameters
# 	  6. function return code
#
# 4. Host source file generation:
#       - File preamble
#       - Include <header> elements of API
#       - Include API header file
#       - For each function generate code:
#         1. Prototype based on function parameters definition
#         2. Allocate local buffer for the parameters structure
#         3. For input/inout parameters: Check value range is given and copy data into parameters struct
#         4. For user buf parameters: Fill memRef table and numOfMemRef
#         5. Invoke DxDI_SepRpcCall()
#         7. For each out "ref" parameter, copy back from message into the respective user parameter.
#         8. Save return code from message
#         9. Free message buffer
#        10. Return the function return code.
#
# 5. SeP source file generation:
#       - File preamble
#       - Include <header> elements of API
#       - Include API header file
#       - For each function generate parsing function to invoke real function with parameters from params struct
#       - SeP Agent entry point (hun) function to switch on FuncId and invoke respective function
#       - SeP Agent (ID) init function
#         Register agent with RPC infrastructure.

import sys
import getopt
import xml.etree.ElementTree as ET
import re
import string
from datetime import datetime
from datetime import date

def usage ():
    print("Usage: " + sys.argv[0] + " --spec=<API spec/XML file> --o-api-h=<API header file path> [--o-host-c=<Host C code file path>] [--o-sep-c=<SeP C code file path>]")

# Parse the command line arguments and return the file names (or None for undefined filename)
# Return: spec_file, api_h_file, host_c_file, sep_c_file
def parse_cmdline_arguments():
    spec_file, api_h_path, host_c_path, sep_c_path = None, None, None, None

    args_spec = [ 'spec=' , 'o-api-h=' , 'o-host-c=' , 'o-sep-c=' ]
    try:
        opts, extra_args = getopt.getopt(sys.argv[1:], "", args_spec)
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err)) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    if len(extra_args) > 0:
        print("Unknown argument:" , extra_args[0])
        usage()
        sys.exit(3)

    for opt, arg in opts:
        if opt == "--spec":
            spec_file = arg
        elif opt == "--o-api-h":
            api_h_path = arg
        elif opt == "--o-host-c":
            host_c_path = arg
        elif opt == "--o-sep-c":
            sep_c_path = arg
        else:
            assert False, "unhandled option"

    if spec_file is None:
        print("API spec. file name is missing")
        usage()
        sys.exit(4)
    if api_h_path is None:
        print("API RPC H file path is missing")
        usage()
        sys.exit(4)

    return spec_file, api_h_path, host_c_path, sep_c_path


# Use sep_rpc_agents.def to look up for this agent ID based on his name
def api_name_to_id(agent_name):
    agent_id = None # default
    # Agents definitions file is located in the same directory as this script
    agents_defs_file_path = re.sub(r"/sep_rpc_gen\.py", "/sep_rpc_agents.def", sys.argv[0])
    agents_defs_file = open(agents_defs_file_path, 'r')
    comment_line_parser = re.compile(r"^\s*#.*$")
    agent_def_parser = re.compile(r"^\s*(\d+)\s*,\s*(\w+)\s*,\s*(\S+)\s*$")
    while True: # Read all defs file lines
        cur_line = agents_defs_file.readline()
        if len(cur_line) == 0:
            break # end of file - not found
        comment_line = comment_line_parser.match(cur_line)
        if comment_line is not None:
            continue # ignore comment
        agent_def = agent_def_parser.match(cur_line)
        if agent_def is None:
            print("Failed parsing:", cur_line)
            sys.exit(1)
        cur_agent_id = agent_def.group(1)
        cur_agent_name = agent_def.group(2)
        if cur_agent_name == agent_name:
            print(cur_line)
            agent_id = cur_agent_id
            break # found
    agents_defs_file.close()
    return agent_id



# Preprocess elements tree into data strcutures that are easier to process
def parse_api_spec (spec_file):
    print("Parsing API spec file" , spec_file , "...")
    api_tree = ET.parse(spec_file);
    #ET.dump(api_tree) # Debug only

    api_root = api_tree.getroot();
    if api_root is None:
        print("Failed finding root element")
        sys.exit(5)

    api_data = {"name" : api_root.get("name")}
    if api_data["name"] is None:
        print("Failed to get API name")
        sys.exit(6)

    api_data["id"] = api_name_to_id(api_data["name"])
    if api_data["id"] is None:
        print("Failed to get SeP RPC Agents Id")
        sys.exit(7)

    api_data["retcodes"] = api_root.find("retcodes")
    if api_data["retcodes"] is None:
        print("Failed to get default return codes")
        sys.exit(8)

    # headers
    api_data["headers"] = []
    for header in api_root.findall("header") :
        api_data["headers"].append(header.text)
    api_data["headers"].append("sep_rpc.h")

    api_data["functions"] = []
    for function in api_root.findall("function") :
        func_data = { "name" : function.get("name") }
        func_data["return"] = function.get("return")
        func_data["retcodes"] = function.find("retcodes") # Optional return codes to override API default <retcodes>
        if func_data["retcodes"] is None: # Use default return codes if not function-specific
            func_data["retcodes"] = api_data["retcodes"]
        func_data["params_buf"] = []
        func_data["params_in"] = []
        func_data["params_inout"] = []
        func_data["params_out"] = []
        func_data["params"] = [] # Simple list of all the parameters as in prototype
        # Scan all parameters and group by direction
        for param in function.findall("param"):
            func_data["params"].append(param)
            param_dir = param.get("direction")
            param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
            if param_type == "buf":
                func_data["params_buf"].append(param)
            elif param_dir == "i":
                func_data["params_in"].append(param)
            elif param_dir == "io":
                func_data["params_inout"].append(param)
            elif param_dir == "o":
                func_data["params_out"].append(param)
            else:
                print("Invalid parameter direction=" , param_dir , "for parameter" , param.get("name"))
                sys.exit(8)
        api_data["functions"].append(func_data)

    # Summary
    print("API" , api_data["name"] , "(ID =" , api_data["id"] , "):" , len(api_data["headers"]) , "headers," , len(api_data["functions"]) , "functions")

    return api_data


# Generate file preamble (license, etc.) common to all source files
def gen_file_preamble (out_file):
    license_str = re.sub(r"<year>", str(date.today().year),
"""/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This file is licensed under the terms provided in the file       *
* libcc54/LICENSE in this directory or a parent directory          *
********************************************************************/\n""")

    # Fetch script revision as given from SVN
    gen_rev = re.sub(r"\$Rev:\s*([0-9]+)\s*\$", r"\1", r"$Rev: 2451 $")

    out_file.write("/* Generated file - DO NOT EDIT! */\n")
    out_file.write("/* Generated by " + sys.argv[0] + "@" + gen_rev + " at " + str(datetime.now()) + " */\n")
    out_file.write(license_str)

## Parse the ptype attribute
#  \param ptype_val The value of "ptype" attribute
#
#  \return (type, array_size, array_size_max)
def parse_ptype (ptype_val):
    data_size = None
    data_size_max = None

    ptype_components = re.match(r"(val|ref|array|buf|wsbuf)(\(.+\))?", ptype_val)
    try:
        param_type = ptype_components.group(1)
    except:
        print("Invalid parameter type from:" , ptype_val)
        sys.exit(9)
    if param_type == "array":
        array_components = re.match(r"\(([^,]+),([^,]+)\)", ptype_components.group(2))
        data_size = array_components.group(1)
        data_size_max = array_components.group(2)
    elif param_type == "buf" or param_type == "wsbuf":
        buf_components = re.match(r"\(([^,]+)\)", ptype_components.group(2))
        data_size = buf_components.group(1)

    return param_type, data_size, data_size_max


## Parse "dtype" attribute
#  \param dtype_val the "dtype" attribute value
#  \return (c_type, endian_type)
def parse_dtype (dtype_val):
    dtype_components = re.match(r"((le|be)(16|32))\(([\w\s]+)\)", dtype_val)
    if dtype_components is None: # No endian data
        endian_type = None
        c_type = dtype_val
    else: # Has endian data
        endian_type = dtype_components.group(1)
        c_type = dtype_components.group(4)

    return c_type, endian_type

## Generate the field of a params struct
# Used by gen_params_struct_fields
# field_prefix may be used (i.e., not "") to denote hierarchy of field inside a structure
def gen_a_param_struct_field (indent_level, field_prefix, param):
    indent_str = "\t" * indent_level
    param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
    c_type , endian_type = parse_dtype(param.get("dtype"))
    field_name = param.get("name")
    # Workspace buffer has no param to pass
    if param_type == "wsbuf":
            field_name += "_size"
            c_type = "DxUint32_t "
    if param_type == "ref" and field_prefix == "": field_prefix = "__" # Copied referenced data is denoted with "__" prefix
    elif param_type == "array": field_name += "[" + array_size_max + "]"
    endian_comment = ("/*" + endian_type + "*/" if endian_type != "" else "") if endian_type is not None else ""
    alignment = param.get("align")
    if alignment is not None:
        alignment_tag = " DX_PAL_COMPILER_ALIGN(" + alignment + ")"
    else:
        alignment_tag = ""

    if param_type == "ref" or param_type == "array":
        flag_str = indent_str + "DxUint8_t " + field_prefix + param.get("name") + "_null_flag;\n"
    else:
        flag_str = ""
    return indent_str + endian_comment + c_type + " " + field_prefix + field_name + alignment_tag + ";\n" + flag_str

# Generate code for parameters structure for given parameters list
# \return struct string
def gen_params_struct_fields (indent_level, field_prefix, params_list):
    return_str = ""
    indent_str = "\t" * indent_level

    for param in params_list:
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        return_str += gen_a_param_struct_field(1, field_prefix, param)

        # Handle second level reference from a struct
        # TODO: Handle unlimited reference levels, i.e., structure references within structures
        fields = param.findall("field")
        for field in fields: # Create place holders for struct fields which are by reference
            if field.get("ptype") == "ref":
                return_str += gen_a_param_struct_field(2,
                    ("__" if param_type == "ref" else "") + param.get("name") + "__", field)
    return return_str


# Generate a string with the given function prototype
def gen_func_prototype (func_name, return_type, the_params):
    proto_str = return_type + " " + func_name + "(\n"
    for param in the_params:
        c_type , endian_type = parse_dtype(param.get("dtype"))
        proto_str += "\t" + c_type + " "
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        if param_type == "buf" or param_type == "ref" or param_type == "wsbuf":
            proto_str += "*"
        proto_str += param.get("name")
        if param_type == "array":
            proto_str += "[]"
        if param is not the_params[-1]: # Is not last parameter?
            proto_str += ",\n"
    proto_str += ")"
    return proto_str


# Generate the RPC header file of the API
def gen_h_file (api_data, base_name, h_file_path):
    api_h_fname = base_name + "_seprpc.h"
    print("Generating API header file at" , h_file_path + "/" + api_h_fname , "...")
    api_file = open(h_file_path + "/" + api_h_fname, 'w')

    ####### File start comments ###########
    gen_file_preamble(api_file)


    api_file.write("/* \\file " + api_h_fname + "\n")
    api_file.write(" * SeP-RPC header file for " + api_data["name"] + " API */\n\n")
    # Guards
    guard_tag = "__" + base_name.upper() + "_SEPRPC_H__"
    api_file.write("#ifndef " + guard_tag + "\n")
    api_file.write("#define " + guard_tag + "\n\n")

    # SRA_ID
    api_file.write("#define SEPRPC_AGENT_ID_"+ api_data["name"] + " " + api_data["id"] + "\n\n");

    # includes
    for header in api_data["headers"] :
        api_file.write("#include \"" + header + "\"\n")
    api_file.write("#include \"dx_pal_compiler.h\"\n")

    function_cnt = 0
    # Per function: function index + params structure
    for function in api_data["functions"] :
        function_cnt += 1
        func_name = function["name"]
        ret_type , endian_type = parse_dtype(function["return"])
        api_file.write("\n\n/************ " + func_name + " ***************/\n");
        api_file.write("#define SEPRPC_FUNC_ID_" + func_name + " " + str(function_cnt) + "\n")
        api_file.write("/*\n" + gen_func_prototype(func_name, ret_type, function["params"]) + "\n*/\n\n")

        params_buf = function["params_buf"]
        # Add a constant for number of user buffer references
        api_file.write("#define SEPRPC_MEMREF_NUM_" + func_name + " " + str(len(params_buf)) + "\n")
        # Compile time test for the memory references limit
        api_file.write("#if SEPRPC_MEMREF_NUM_" + func_name + " > SEP_RPC_MAX_MEMREF_PER_FUNC\n")
        api_file.write("#error SEPRPC_MEMREF_NUM_" + func_name + " is more than SEP_RPC_MAX_MEMREF_PER_FUNC\n")
        api_file.write("#endif\n")
        # Index of each buffer reference in the memRef array
        memref_idx = 0
        for param in params_buf :
            api_file.write("#define SEPRPC_MEMREF_IDX_" + param.get("name") + " " + str(memref_idx) + "\n")
            memref_idx += 1
        api_file.write("\n")

        # Create the parameters structure ("Message")
        api_file.write("typedef struct SepRpc_" + func_name + "Params {\n")

        # User buffers first
        api_file.write("\tuint32_t num_of_memrefs; /* Number of elements in the memRef array */\n")
        if len(params_buf) > 0: # memRef array exists only if numOfMemRef > 0
            api_file.write("\tstruct seprpc_memref memref[SEPRPC_MEMREF_NUM_" + func_name + "];\n")

        if len(function["params_in"]) > 0:
            api_file.write("\t/* Input */\n")
            api_file.write(gen_params_struct_fields(1, "", function["params_in"]))
        if len(function["params_inout"]) > 0:
            api_file.write("\t/* Input/Output */\n")
            api_file.write(gen_params_struct_fields(1, "", function["params_inout"]))
        if len(function["params_out"]) > 0:
            api_file.write("\t/* Output */\n")
            api_file.write(gen_params_struct_fields(1, "", function["params_out"]))
        if function["return"] != "void":
            ret_type , endian_type = parse_dtype(function["return"])
            api_file.write("\t" + ret_type + " _funcRetCode;\n")

        api_file.write("} SepRpc_" + func_name + "Params_s;\n\n")
    # End "foreach function"

    # Close guards
    api_file.write("\n#endif /*" + guard_tag + "*/\n")

    api_file.close()


## Generate range validation code for given parameter
# \param indent_str The base indentation string
# \param param_val The parameter reference string
# \param param_range The paramter "range" attribute
# \param inval_param_retcode The return code to use in case of validation failure
#
# \return (A string of the generated code , iterator depth = 0 if no iterator or indent_depth if use iterator)
def gen_a_param_verify_code (indent_depth, ref_prefix, param, inval_param_retcode):
    iterator = "i" + str(indent_depth)
    iter_depth = 0 # 0 until uses an iterator or gets a value from recursive invocation
    verify_code = ""

    param_range = param.get("range")
    if param_range is not None:
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        param_val = ref_prefix + param.get("name")
        if param_type == "ref": param_val = "*(" + param_val + ")"


        if param_type == "array" : # Loop on array elements
            verify_code += "\t" * indent_depth + "for (" + iterator + " = 0; " + iterator + " < (" + array_size + "); " + iterator + "++) {\n"
            indent_depth += 1
            param_val += "[" + iterator + "]"
            iter_depth = indent_depth

        # Generate boolean expression for given range limitation
        range_components = re.search(r"([\(\[])(\w*),(\w*)([\)\]])", param_range)
        #print(range_components.group(1), range_components.group(2), range_components.group(3), range_components.group(4))
        min_val = range_components.group(2)
        min_eq = "=" if range_components.group(1) == "[" else ""
        max_val = range_components.group(3)
        max_eq = "=" if range_components.group(4) == "]" else ""
        range_bool_expr = ""
        if min_val != "":
            range_bool_expr = "(" + param_val + " >" + min_eq + " " + min_val + ")"
        if max_val != "":
            range_bool_expr = range_bool_expr + " && (" + param_val + " <" + max_eq + " " + max_val + ")"

        if range_bool_expr != "":
            verify_code += "\t" * indent_depth + "SEP_RPC_ASSERT(" + range_bool_expr + ", " + inval_param_retcode + "); /* " + param_range + " */\n"

        if param_type == "array" :
            indent_depth -= 1
            verify_code += "\t" * indent_depth + "}\n"


    return verify_code , iter_depth

## Get the reference expression of given parameter in the params structure
# \return The reference expression to append to "params." or "params->"
def get_params_struct_ref (ref_prefix, param_name, param_type):
    struct_ref_prefix = re.sub(r"(\w+)(->)", r"__\1", ref_prefix)
    if param_type == "ref":
            return struct_ref_prefix + "__" + param_name
    # Not ref - direct access
    return struct_ref_prefix + ("." if struct_ref_prefix != "" else "") + param_name

# Generate code for copying data into the parameters structure
#
# \param indent_depth Indentation depth
# \param param_ref Parameter reference string (e.g., structure reference)
# \param dtype_val The value of the "dtype" attribute of the parameter item
# \param direction The copy direction. "in" for input parameters into the params structure, and "out" for vice-versa.
#
# \return (The copy code , iterator depth = 0 if no iterator or indent_depth if use iterator)
def gen_a_param_copy_code(indent_depth, ref_prefix, param, direction):
    iterator = "i" + str(indent_depth - 1)
    return_str = ""
    iter_depth = 0 # 0 until uses an iterator or gets a value from recursive invocation

    c_type , endian_type = parse_dtype(param.get("dtype"))
    param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))

    # Construct the params structure reference matching a given parameter
    struct_param_value = "params." + get_params_struct_ref(ref_prefix, param.get("name"), param_type)
    func_param_value = ("*(" if param_type == "ref" else "") + ref_prefix + param.get("name") + (")" if param_type == "ref" else "")
    if param_type == "array" and endian_type is not None : # Endianess fix requires looping over array items
        struct_param_value += "[" + iterator + "]"
        func_param_value += "[" + iterator + "]"

    # Calculate lvalue and rvalue
    if direction == "in":
        lvalue = struct_param_value
        if endian_type is None:
            rvalue = func_param_value
        else:
            rvalue = "cpu_to_" + endian_type + "(" + func_param_value + ")"

    else: # "out"
        lvalue = func_param_value
        if endian_type is None:
            rvalue = struct_param_value
        else:
            rvalue = endian_type + "_to_cpu(" + struct_param_value + ")"

    # Generate code
    if param_type == "array" :
        if endian_type is None: # Use memcpy
            return_str += "\t" * indent_depth + "memcpy(" + lvalue + ", " + rvalue + ", sizeof(" + c_type + ") * (" + array_size + "));\n"
        else: # Loop on array elements in order to switch endianess of each
            return_str += "\t" * indent_depth + "for (" + iterator + " = 0; " + iterator + " < (" + array_size + "); " + iterator + "++) {\n\t"
            indent_depth += 1
            return_str += "\t" * indent_depth + lvalue + " = " + rvalue + ";\n" # lvalue and rvalue already include the array index reference
            indent_depth -= 1
            iter_depth = indent_depth;
            return_str += "\t" * indent_depth + "}\n"
    elif param_type == "wsbuf":
            return_str += "\t" * indent_depth + "params." + param.get("name") + "_size = cpu_to_le32((" + array_size + "));\n"
    else: # plain assignment
            return_str += "\t" * indent_depth + lvalue + " = " + rvalue + ";\n"

    return return_str , iter_depth


## Generate code for parameters sizes and ranges validation in host wrapper code
#
# \param base_level indicates we are in the root of this function (no recursion)
# \param direction The direction ("in" or "out") of the parameters for this wrapper code
# \param ref_prefix A prefix string for structure fields (structure references)
# \param indent_depth The indentation depth (for array iterators)
# \param params_list A list of parameters/fields elements to validate (if have "range" attribute)
# \param inval_param_retcode The return code to use in case of validation failure
#
# \return A string of the generated code
def gen_params_code (base_level, direction, ref_prefix, indent_depth, params_list, inval_param_retcode):
    return_str = ""
    code_indent = "\t" * indent_depth
    iterator = "i" + str(indent_depth)
    max_iter_depth = 0

    for param in params_list:
        param_name = param.get("name")
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        fields = param.findall("field")
        is_a_struct = (len(fields) > 0)
        iter_depth = 0

        #param_full_name = ("__" if param_type == "ref" else "") + ref_prefix + param.get("name")
        param_full_name = get_params_struct_ref(ref_prefix,param.get("name"),param_type)
        if is_a_struct: # A struct with fields that needs to be verified
            # Invoke this function in recursion
            if (base_level == 1 and param_type == "array") or param_type == "ref":
                return_str += code_indent + "if (" + param.get("name") + " == NULL) {\n" + "\t\tparams." + param_full_name + "_null_flag = 1;\n" + "\t} else {\n"

            params_code , iter_depth = gen_params_code(0, direction,
                                          ref_prefix + param_name + ("." if param_type == "val" else "->"),
                                          indent_depth + (2 if param_type == "array" else 0) + (1 if param_type == "ref" else 0),
                                          fields, inval_param_retcode)
        else: # Not a struct
            params_code = ""
            param_direction = param.get("direction")

            if (base_level == 1 and param_type == "array") or param_type == "ref":
                struct_name = ref_prefix + param.get("name")
                return_str += code_indent + "if (" + struct_name + " == NULL) {\n"
                return_str += code_indent + "\tparams." + param_full_name + "_null_flag = 1;\n"
                return_str += code_indent + "} else {\n"
                indent_depth += 1
                code_indent = "\t" * indent_depth

            # Handle input parameters in case of "in" direction
            if direction == "in" and (param_direction == "i" or param_direction == "io"):
                # Only input parameters are validated for range
                verify_code , verify_iter_depth = gen_a_param_verify_code(indent_depth, ref_prefix, param, inval_param_retcode)
                copy_code , copy_iter_depth = gen_a_param_copy_code(indent_depth, ref_prefix, param, "in")
                params_code += verify_code + copy_code
                iter_depth = copy_iter_depth if verify_iter_depth < copy_iter_depth else verify_iter_depth
            # Handle output paramters in case of "out" direction
            elif direction == "out" and (param_direction == "o" or param_direction == "io"):
                copy_code , iter_depth = gen_a_param_copy_code(indent_depth, ref_prefix, param, "out")
                params_code += copy_code

        if direction == "in": # The array size is validated for all paramters in the input parameters valiadtion phase
            if param_type == "array" and array_size_max != array_size: # Requires run time array size verification
                return_str += code_indent + "/* Verify array size of " + param_name + " */\n"
                return_str += code_indent + "SEP_RPC_ASSERT((" + array_size + ") <= (" + array_size_max + "), " + inval_param_retcode + ");\n"

        return_str += params_code

        if (base_level == 1 and param_type == "array") or param_type == "ref":
            if not is_a_struct:
                indent_depth -= 1
            code_indent = "\t" * indent_depth
            return_str += code_indent + "\tparams." + param_full_name + "_null_flag = 0;\n"
            return_str += code_indent + "}\n\n"

        if max_iter_depth < iter_depth:
            max_iter_depth = iter_depth

    return return_str , max_iter_depth


## Generate memory references setup code
def gen_memref_setup_code(params_list):
    return_str = ""
    for memref in params_list:
        param_type, buf_size, void = parse_ptype(memref.get("ptype"))
        if param_type != "buf":
            print("Invalid ptype -", param_type, "- for parameter", memref.get("name"))
        direction = memref.get("direction")
        if direction == "i": dma_dir = "DXDI_DATA_TO_DEVICE"
        elif direction == "io": dma_dir = "DXDI_DATA_BIDIR"
        elif direction == "o": dma_dir = "DXDI_DATA_FROM_DEVICE"
        else:
            print("Invalid memory reference direction -", direction, "- for parameter", memref.get("name"))
            sys.exit(9)
        memref_idx = "SEPRPC_MEMREF_IDX_" + memref.get("name")
        return_str += "\t/* " + memref.get("name") + " */\n"
        return_str += "\tmemRefs[" + memref_idx + "].start_or_offset = (unsigned long)" + memref.get("name") + ";\n"
        return_str += "\tmemRefs[" + memref_idx + "].size = " + buf_size + ";\n"
        return_str += "\tmemRefs[" + memref_idx + "].dma_direction = " + dma_dir + ";\n"
        return_str += "\tmemRefs[" + memref_idx + "].ref_id = DXDI_MEMREF_ID_NULL;\n"

    return return_str

# Generate the host side API wrapper
def gen_host_c_file (api_data, base_name, c_file_path):
    host_c_fname = base_name + "_seprpc_stub.c"
    print("Generating host C file at" , c_file_path + "/" + host_c_fname , "...")
    host_c_file = open(c_file_path + "/" + host_c_fname, 'w')

    ####### File start comments ###########
    gen_file_preamble(host_c_file)

    host_c_file.write("/* \\file " + host_c_fname + "\n")
    host_c_file.write(" * SeP-RPC host wrappers/stubs implementation for " + api_data["name"] + " API */\n\n")

    # includes
    host_c_file.write("#include <string.h>\n")
    for header in api_data["headers"] :
        host_c_file.write("#include \"" + header + "\"\n")
    host_c_file.write("#include \"driver_interface.h\"\n") # DriverInterface for DxDI_SepRpcCall
    host_c_file.write("#include \"" + base_name + "_seprpc.h\"\n") # RPC API generated header file

    # functions
    for function in api_data["functions"] :
        func_name = function["name"]
        ret_type , endian_type = parse_dtype(function["return"])
        host_c_file.write("\n\n/************ " + func_name + " ***************/\n");
        # Function prototype
        host_c_file.write(gen_func_prototype(func_name, ret_type, function["params"]) + "\n{\n")
        host_c_file.write("\tDxDI_RetCode_t diRc;\n")
        host_c_file.write("\tseprpc_retcode_t rpcRc;\n")
        numOfMemRefs = len(function["params_buf"])
        if numOfMemRefs > 0: # Mem. Ref. data
            host_c_file.write("\tstruct dxdi_memref memRefs[SEPRPC_MEMREF_NUM_" + func_name + "];\n")
        # Local buffer for parameters structure
        host_c_file.write("\tstruct SepRpc_" + func_name + "Params params;\n")

        # Generate parameters handling code (copy in/out and verify)
        inparams_code , max_in_iter_depth = gen_params_code(1,"in", "", 1,
                         function["params_in"] + function["params_inout"] + function["params_out"],
                         function["retcodes"].get("invalid_param"))
        outparams_code , max_out_iter_depth = gen_params_code(1,"out", "", 1,
                         function["params_inout"] + function["params_out"],
                         function["retcodes"].get("invalid_param"))
        # Generate iterators declaration code (if any)
        if (max_in_iter_depth > max_out_iter_depth):
            max_iter_depth = max_in_iter_depth
        else:
            max_iter_depth = max_out_iter_depth
        if max_iter_depth > 0:
            for i in range(1, max_iter_depth):
                host_c_file.write("\tunsigned int i" + str(i) + ";\n")
        host_c_file.write("\n")

        if inparams_code != "":
            host_c_file.write("\t/* Verify input parameters and copy into params. struct. */\n" + inparams_code + "\n")
        del inparams_code # Free memory

        memref_code = gen_memref_setup_code(function["params_buf"])
        if memref_code != "":
            host_c_file.write("\t/* User DMA buffers (memory references) */\n" + memref_code + "\n")
            memref_param_str = "memRefs"
        else:
            memref_param_str = "NULL"
        del memref_code # Free memory

        # Set number of mem. refs.
        host_c_file.write("\tparams.num_of_memrefs = cpu_to_le32(" + "SEPRPC_MEMREF_NUM_" + func_name + ");\n\n")

        # Invoke IOCTL
        host_c_file.write("\tdiRc = DxDI_SepRpcCall(SEPRPC_AGENT_ID_" + api_data["name"] +
            ", SEPRPC_FUNC_ID_" + func_name + ",\n\t\t" + memref_param_str +
            ", sizeof(params), (struct seprpc_params*)&params, &rpcRc);\n\n")
        # Assert DxDI_SepRpcCall invocation success
        host_c_file.write("\tSEP_RPC_ASSERT(diRc == DXDI_RET_OK, " + function["retcodes"].get("generic_error") + ");\n")

        # Handle return code
        host_c_file.write("\tif (rpcRc != SEPRPC_RET_OK) {\n")
        host_c_file.write("\t\tSEP_RPC_LOG(\"RPC of " + func_name + " failed with RPC error code %d\\n\", rpcRc);\n")
        if function["return"] != "void":
            host_c_file.write("\t\tswitch (rpcRc) {\n")
            host_c_file.write("\t\tcase SEPRPC_RET_EINVAL:\n\t\t\treturn " + function["retcodes"].get("invalid_param") + ";\n")
            host_c_file.write("\t\tcase SEPRPC_RET_ENORSC:\n\t\t\treturn " + function["retcodes"].get("not_enough_resources") + ";\n")
            host_c_file.write("\t\tdefault:\n\t\t\treturn " + function["retcodes"].get("generic_error") +";\n")
            host_c_file.write("\t\t}\n")
        host_c_file.write("\t}\n\n")
        # Copy data out
        if outparams_code != "":
            host_c_file.write("\t/* Copy back output parameters */\n" + outparams_code + "\n")
        del outparams_code # Free memory
        if function["return"] != "void":
            c_type , endian_type = parse_dtype(function["return"])
            host_c_file.write("\treturn " + endian_type +"_to_cpu(params._funcRetCode);\n")

        host_c_file.write("} /* " + func_name + " */\n\n") # End of function

    host_c_file.close()

## Generate "parser" function of a specific API function for SeP agent
def gen_sep_rpc_func_parser (function):
    func_name = function["name"]
    func_code = "static inline seprpc_retcode_t SepRpc_" + func_name + "Parser(\n"
    func_code += "\tstruct SepRpc_" + func_name + "Params *params,\n"
    func_code += "\tDX_DmaObjHandle_t dmaObjs[])\n{\n"

    # Set references (pointers) in structures
    for param in function["params"]:
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        fields = param.findall("field")
        for field in fields:
            field_type, field_array_size, field_array_size_max = parse_ptype(field.get("ptype"))
            if field_type == "ref": # Put the pointer to the ref. field copy into the pointer field
                field_ref = ("__" if param_type == "ref" else "") + param.get("name") + "." + field.get("name")
                func_code += "\tif (params->__" + param.get("name") + "__" + field.get("name") + "_null_flag == 0) {\n"
                func_code += "\t\tparams->" + field_ref + " = &(params->__" + param.get("name") + "__" + field.get("name") + ");\n\t} else {\n"
                func_code += "\t\tparams->" + field_ref + " = NULL;\n\t}\n"
    # Add workspace buffer size check
    for param in function["params"]:
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        if param_type == "wsbuf":
            func_code += "\tif (params->" + param.get("name") + "_size > SEP_RPC_MAX_WORKSPACE_SIZE)\n"
            func_code += "\t\treturn SEPRPC_RET_ENORSC;\n\n"

    # Generate function invocation code
    func_code += "\t"
    if function["return"] != "void":
        func_code += "params->_funcRetCode = "
    func_code += func_name + "(\n"
    for param in function["params"]:
        param_name = param.get("name")
        param_type, array_size, array_size_max = parse_ptype(param.get("ptype"))
        if param_type == "ref" or param_type == "array":
            func_code += "\t\tparams->" + ("__" if param_type == "ref" else "") + param_name + "_null_flag != 0 ? NULL :"
        if param_type == "buf": # Give DMA object for that parameter
            func_code += "\t\t(" + param.get("dtype") + " *)(dmaObjs[SEPRPC_MEMREF_IDX_" + param_name + "])"
        elif param_type == "ref": # Dereference copy in params struct
            func_code += "&(params->__" + param_name + ")"
        elif param_type == "wsbuf":
            func_code += "\t\t(" + param.get("dtype") + " *)getWorkspacePtr()"
        else: # Give the field in "params" directly
            func_code += "\t\tparams->" + param_name
        if param is not function["params"][-1]: # Is not last parameter?
            func_code += ",\n"
    func_code += ");\n"
    # End parser function
    func_code += "\n\treturn SEPRPC_RET_OK;\n"
    func_code += "}\n"
    return func_code


# Generate the SeP agent implementation C source file
def gen_sep_c_file(api_data, base_name, c_file_path):
    sep_c_fname = base_name + "_seprpc_agent.c"
    print("Generating SeP C file at" , c_file_path + "/" + sep_c_fname , "...")
    sep_c_file = open(c_file_path + "/" + sep_c_fname, 'w')

    ####### File start comments ###########
    gen_file_preamble(sep_c_file)

    sep_c_file.write("/* \\file " + sep_c_fname + "\n")
    sep_c_file.write(" * SeP-RPC agent implementation for " + api_data["name"] + " API */\n\n")

    # includes
    for header in api_data["headers"] :
        sep_c_file.write("#include \"" + header + "\"\n")
    sep_c_file.write("#include \"sep_log.h\"\n")
    sep_c_file.write("#include \"sep_rpc_server.h\"\n") # SeP RPC agent infrastructure API (available at sep/include)
    sep_c_file.write("#include \"" + base_name + "_seprpc.h\"\n\n") # RPC API generated header file

    # Parser functions
    for function in api_data["functions"] :
        sep_c_file.write(gen_sep_rpc_func_parser(function) + "\n");

    # "Hub" function (agent's entry point)
    hub_func_name = "SepRpc_" + api_data["name"] + "_Agent"
    sep_c_file.write("DX_PAL_COMPILER_FUNC_DONT_INLINE static seprpc_retcode_t " + hub_func_name + "_Parser" + "(\n")
    sep_c_file.write("\tseprpc_agent_id_t agentId,\n")
    sep_c_file.write("\tseprpc_func_id_t funcId,\n")
    sep_c_file.write("\tuint16_t paramsMsgSize,\n")
    sep_c_file.write("\tstruct seprpc_params *params,\n")
    sep_c_file.write("\tDX_DmaObjHandle_t dmaObjs[])\n{\n")

    # Function ID based switch statement
    sep_c_file.write("\tswitch (funcId) {\n")

    # static functions to be invoked by the entry-point function
    for function in api_data["functions"] :
        func_name = function["name"]
        sep_c_file.write("\tcase SEPRPC_FUNC_ID_" + func_name + ":\n");
        # Verify HMB size for requested params structure
        sep_c_file.write("\t\tif (paramsMsgSize < sizeof(struct SepRpc_" + func_name + "Params)) {\n")
        sep_c_file.write("\t\t\tSEP_LOG_ERR(\"Invalid HMB size for struct SepRpc_" + func_name + "Params\\n\");\n")
        sep_c_file.write("\t\t\treturn SEPRPC_RET_EINVAL;\n")
        sep_c_file.write("\t\t}\n")
        sep_c_file.write("\t\treturn SepRpc_" + func_name + "Parser((struct SepRpc_" + func_name + "Params *)params, dmaObjs);\n\n")

    # End of switch statement
    sep_c_file.write("\t} /* switch (funcId) */\n\n")

    # Error return code - not supposed to reach here (all functions are supposed to be handled in the switch statement
    sep_c_file.write("\treturn SEPRPC_RET_EINVAL_FUNC;\n")
    # End of "hub" function
    sep_c_file.write("}\n\n");

    # Wrapper function for d-cache stack replacement
    hub_func_name = "SepRpc_" + api_data["name"] + "_Agent"
    sep_c_file.write("static seprpc_retcode_t " + hub_func_name + "(\n")
    sep_c_file.write("\tseprpc_agent_id_t agentId,\n")
    sep_c_file.write("\tseprpc_func_id_t funcId,\n")
    sep_c_file.write("\tuint16_t paramsMsgSize,\n")
    sep_c_file.write("\tstruct seprpc_params *params,\n")
    sep_c_file.write("\tDX_DmaObjHandle_t dmaObjs[])\n{\n")

    sep_c_file.write("\tregister seprpc_retcode_t rc = 0;\n")
    sep_c_file.write("\n\tRPC_CHANGE_STACK();\n")

    sep_c_file.write("\n\trc = " + hub_func_name + "_Parser(\n")
    sep_c_file.write("\t\tagentId,\n")
    sep_c_file.write("\t\tfuncId,\n")
    sep_c_file.write("\t\tparamsMsgSize,\n")
    sep_c_file.write("\t\tparams,\n")
    sep_c_file.write("\t\tdmaObjs);\n\n")

    sep_c_file.write("\tRPC_RESTORE_STACK();\n\n")

    sep_c_file.write("\treturn rc;\n}\n\n")


    # Init. function for RPC agent: Register to RPC infrastructure
    sep_c_file.write("seprpc_retcode_t SepRpc_Init" + api_data["name"] + "(void)\n{\n")
    sep_c_file.write("\treturn SepRpc_RegisterHandler(" + api_data["id"] + ", " + hub_func_name + ", \"" + api_data["name"] + "\");\n")
    sep_c_file.write("}\n")

    sep_c_file.close()


def main ():
    spec_file , api_h_path , host_c_path , sep_c_path = parse_cmdline_arguments()
    print("spec_file=" , spec_file , "  api_h_path=" , api_h_path , "  host_c_path=" , host_c_path , "  sep_c_path=" , sep_c_path)
    # Base name for all the files is taken from spec file name
    spec_basename = re.sub(r".*/" , "", spec_file) # Remove leading directories
    spec_basename = re.sub(r"\.[^/.]*$", "" , spec_basename) # Remove .xml suffix

    my_api = parse_api_spec(spec_file)

    if api_h_path is not None:
        gen_h_file(my_api, spec_basename, api_h_path)

    if host_c_path is not None:
        gen_host_c_file(my_api, spec_basename, host_c_path)

    if sep_c_path is not None:
        gen_sep_c_file(my_api, spec_basename, sep_c_path)

    print("Done.")


#############################
if __name__ == "__main__":
    main()
