import os
import sys
import dis
import types
import inspect
import StringIO
import importlib
import xlog_module
xlog_module.xlog("1")

def hook_decorator(original_function):
    def wrapper(*args, **kwargs):
        message = "【args】 {} ".format(args)
        xlog_module.xlog(message)
        result = original_function(*args, **kwargs)
        return result

    return wrapper

import gclient.login.login_util

login_util_module = sys.modules["gclient.login.login_util"]

if "HOOKED_FUNCTIONS" not in sys.modules:
    sys.modules["HOOKED_FUNCTIONS"] = set()

for attr_name in dir(login_util_module):
    attr_value = getattr(login_util_module, attr_name)
    # message = "[0 Attributes ]attr_name: {} attr_value: {}".format(attr_name, attr_value)
    # xlog_module.xlog(message)
    if isinstance(attr_value, types.FunctionType) and attr_name == "DoRequest" and attr_name not in sys.modules["HOOKED_FUNCTIONS"]:
        setattr(login_util_module, attr_name, hook_decorator(attr_value))
        sys.modules["HOOKED_FUNCTIONS"].add(attr_name) 
        # argspec = inspect.getargspec(attr_value)
        # message = "[0 argspec ]: {} ".format(argspec)
        # xlog_module.xlog(message)

# for module_name, module in sys.modules.items():
    
#     if module is None:
#         pass
#         # try:
#         #     module = importlib.import_module(module_name)
#         #     message = "[x]Reloaded module: {module_name}".format(module_name)
#         #     xlog_module.xlog(message)
#         # except Exception as e:
#         #     message = "[x]Failed to reload module: {} Error: {}".format(module_name, e)
#         #     xlog_module.xlog(message)
#     else:
#         if "login" in module_name:
#             message = "[0]Module name: {}Module object: {}".format(module_name, module)
#             xlog_module.xlog(message)
#             for attr_name in dir(module):
#                 attr_value = getattr(module, attr_name)
#                 message = "[0 Attributes ]attr_name: {} attr_value: {}".format(attr_name, attr_value)
#                 xlog_module.xlog(message)
#                 # if isinstance(attr_value, types.FunctionType):
#                 #     argspec = inspect.getargspec(attr_value)
#                 #     message = "[0 argspec ]: {} ".format(argspec)
#                 #     xlog_module.xlog(message)
#                 #     try:
#                 #         source_code = inspect.getsource(attr_value)
#                 #         message = "[0 source_code ]: {} ".format(source_code)
#                 #         xlog_module.xlog(message)
#                 #     except Exception as e:
#                 #         message = "[x]Error: {}".format(e)
#                 #         xlog_module.xlog(message)
#                 #         buffer = StringIO.StringIO()
#                 #         sys.stdout = buffer
#                 #         dis.dis(attr_value)
#                 #         sys.stdout = sys.__stdout__

#                 #         dis_output = buffer.getvalue()
#                 #         message = "[dis]{}".format(dis_output)
#                 #         xlog_module.xlog(message)
                        
# # for var_name, var_value in globals().items():
# #     message = "[1]var_name: {}var_value: {}".format(var_name, var_value)
# #     xlog_module.xlog(message)

# # for var_name, var_value in vars(os).items():
# #     message = "[2]var_name: {}var_value: {}".format(var_name, var_value)
# #     xlog_module.xlog(message)