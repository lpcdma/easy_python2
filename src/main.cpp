#include <dobby.h>
#include <iostream>

#include <unistd.h>
#include <dlfcn.h>

#include <thread>

#include "elf_util.h"
#include "Hexdump.hpp"
#include "Sig.hpp"

#include "logging.h"

typedef int (*t__android_log_write)(int prio, const char *tag,  const char *msg);
t__android_log_write sr__android_log_write = nullptr;

extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt,...)
{
    va_list ap;  
	char buf[LOG_BUF_SIZE] = {0};
  
    va_start(ap, fmt);  
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);  
    va_end(ap);

	return sr__android_log_write(prio,tag,buf);
}

//////////////////////////////////

#define METH_OLDARGS  0x0000
#define METH_VARARGS  0x0001
#define METH_KEYWORDS 0x0002

#define Py_single_input 256
#define Py_file_input 257
#define Py_eval_input 258

typedef struct {
    int cf_flags;
} PyCompilerFlags;

typedef void *(*PyCFunction)(void *, void *);
struct PyMethodDef {
    const char	*ml_name;	/* The name of the built-in function/method */
    PyCFunction  ml_meth;	/* The C function that implements it */
    int		 ml_flags;	/* Combination of METH_xxx flags, which mostly
				   describe the args expected by the C func */
    const char	*ml_doc;	/* The __doc__ attribute, or NULL */
};

typedef void* (*t_PyRun_StringFlags)(const char *str, int start, void *globals, void *locals, PyCompilerFlags *flags);
t_PyRun_StringFlags PyRun_StringFlags = nullptr;

typedef void* (*t_PyEval_GetLocals)();
t_PyEval_GetLocals PyEval_GetLocals = nullptr;

typedef void* (*t_PyEval_GetGlobals)();
t_PyEval_GetGlobals PyEval_GetGlobals = nullptr;

typedef void (*t_PyErr_PrintEx)(int set_sys_last_vars);
t_PyErr_PrintEx PyErr_PrintEx = nullptr;

typedef void* (*t_PyImport_AddModule)(const char *name);
t_PyImport_AddModule PyImport_AddModule = nullptr;

typedef void* (*t_PyModule_GetDict)(void* module);
t_PyModule_GetDict PyModule_GetDict = nullptr;

typedef int (*t_PyArg_ParseTuple)(void *args, const char *format, ...);
t_PyArg_ParseTuple PyArg_ParseTuple = nullptr;

typedef void* (*t_Py_InitModule3)(char *name, PyMethodDef *methods, char *doc);
t_Py_InitModule3 Py_InitModule3 = nullptr;

typedef int (*t_Py_FlushLine)();
t_Py_FlushLine Py_FlushLine = nullptr;

typedef void* (*t_PyErr_Clear)();
t_PyErr_Clear PyErr_Clear = nullptr;

static void* py_xlog(void* self, void* args)
{
    const char* message;

    if (!PyArg_ParseTuple(args, "s", &message))
        return NULL;

    LOGI("[xlog]{}", message);

    return self;
}

static PyMethodDef XlogMethods[] = {
    {"xlog", py_xlog, METH_VARARGS, "Call xlog function"},
    {NULL, NULL, 0, NULL}
};

struct MyRet;
typedef void (*FunctionPointerType)(MyRet*);
struct MyRet {
    void* data1;
    uint64_t data2;
    void* data3;
    uint64_t data4;
    void* data5;
    void* data6;
    uint64_t data7;
    void* data8;
    void* data9;
    void* data10;
    void* data11;
    void* data12;
    void* data13;
    FunctionPointerType function_pointer;
};

static std::string newStr = "";
static bool log_inited = false;
static bool injected = false;

void* (*old_PyRun_StringFlags)(const char *str, int start, void *globals, void *locals, PyCompilerFlags *flags);
void* my_PyRun_StringFlags(const char *str, int start, void *globals, void *locals, PyCompilerFlags *flags)
{
    if (!log_inited)
    {
        log_inited = true;
        Py_InitModule3("xlog_module", XlogMethods, nullptr);
    }
    
    if (start == Py_file_input)
    {
        
        // LOGI("{}", str);

        if (!injected && str && strstr(str, "#125478") != nullptr)
        {
            LOGI("----------------");
            injected = true;
            const char *code = "\nfilename = \"/data/data/com.netease.yhtj/files/hook.py\"\nwith open(filename, \"r\") as file:\n    code = file.read()\nexec(code)\n";
            // const char *code = "\nimport xlog_module\nxlog_module.xlog(\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\")\n\n";

            void* main_module = PyImport_AddModule("__main__");
            LOGI("main_module {}", main_module);
            void* main_dict = PyModule_GetDict(main_module);
            LOGI("main_dict {}", main_dict);

            PyCompilerFlags cf;

            MyRet* myret = (MyRet*)old_PyRun_StringFlags(code, Py_file_input, main_dict, main_dict, &cf);
            LOGI("myret {}", (void*)myret);

            // hexdump((unsigned char*)myret, 0x100);
            // if (myret == nullptr)
            // {
            //     PyErr_PrintEx(0);
            // }
            // if (Py_FlushLine())
            // {
            //     PyErr_Clear();
            // }

            // LOGD("1");
            // newStr = std::string(str);
            // newStr.append(code);
            // LOGD("2");
            // LOGD("{}", newStr.substr(newStr.length() - 1024));
            // return old_PyRun_StringFlags(newStr.c_str(), start, globals, locals, flags);
        }
    }
    return old_PyRun_StringFlags(str, start, globals, locals, flags);
}

//////////////////////////////////

static void* load_dll(void*){

    LOGD("load_dll ");

    sleep(3);

    auto main_lib_elf = SandHook::ElfImg("libGame.so");
    void* base = (void*)main_lib_elf.getModuleBase();
    LOGD("base {}", base);

    uint64_t size = (uint64_t)main_lib_elf.getModuleSize();
    LOGD("size {}", size);

    // const void * find_ret = Sig::find(base, size, "FF ? ? ? FC 6F 05 A9 FA 67 06 A9 F8 5F 07 A9 F6 57 08 A9 F4 4F 09 A9 FD 7B 0A A9 FD ? ? ? 21 ? ? ?");
    // LOGD("find_ret {}", find_ret);

    PyRun_StringFlags = reinterpret_cast<t_PyRun_StringFlags>(reinterpret_cast<std::uintptr_t>(base) + 0x2101714); // <string>  PyAST_FromNode ncoding declaration in Unicode string
    hexdump((unsigned char*)PyRun_StringFlags, 0x20);
    PyEval_GetLocals = reinterpret_cast<t_PyEval_GetLocals>(reinterpret_cast<std::uintptr_t>(base) + 0x20DF180); // vars(): no locals!?  -> builtin_vars -> __dict__/vars
    PyEval_GetGlobals = reinterpret_cast<t_PyEval_GetGlobals>(reinterpret_cast<std::uintptr_t>(base) + 0x20DF1C4); // __import__ PyImport_Import __doc__ -> PyEval_GetGlobals
    PyErr_PrintEx = reinterpret_cast<t_PyErr_PrintEx>(reinterpret_cast<std::uintptr_t>(base) + 0x21017E4);

    PyImport_AddModule = reinterpret_cast<t_PyImport_AddModule>(reinterpret_cast<std::uintptr_t>(base) + 0x20F3A18); //last_type
    PyModule_GetDict = reinterpret_cast<t_PyModule_GetDict>(reinterpret_cast<std::uintptr_t>(base) + 0x2079C18); //_ast

    PyArg_ParseTuple = reinterpret_cast<t_PyArg_ParseTuple>(reinterpret_cast<std::uintptr_t>(base) + 0x20EF300);//O(ziiz)
    Py_InitModule3 = reinterpret_cast<t_Py_InitModule3>(reinterpret_cast<std::uintptr_t>(base) + 0x20FBBA4); //_ast

    Py_FlushLine = reinterpret_cast<t_Py_FlushLine>(reinterpret_cast<std::uintptr_t>(base) + 0x20DF2BC); //lost sys.stderr
    PyErr_Clear = reinterpret_cast<t_PyErr_Clear>(reinterpret_cast<std::uintptr_t>(base) + 0x20EBD14); //lost sys.stderr

    DobbyHook((void*)PyRun_StringFlags, std::bit_cast<void*>(&my_PyRun_StringFlags), (void**)&old_PyRun_StringFlags);

    //PyEval_EvalCodeEx: NULL globals

    return nullptr;
}

void hook_entry() __attribute__((constructor));
void hook_entry()
{
    if (sizeof(void*) == 8)
    {
        void * liblog_handle = dlopen("/system/lib64/liblog.so", RTLD_LAZY);///system/lib/arm/liblog.so
        if (liblog_handle)
        {
            sr__android_log_write = (t__android_log_write)dlsym(liblog_handle,"__android_log_write");
        }

        LOGD("enter hook 64!!!");
        // std::jthread worker(load_dll, nullptr);
        pthread_t tid;
        pthread_create(&tid, NULL, load_dll, NULL);
        LOGD("enter hook 64***");
    } else {
        void * liblog_handle = dlopen("/system/lib/liblog.so", RTLD_LAZY);
        if (liblog_handle)
        {
            sr__android_log_write = (t__android_log_write)dlsym(liblog_handle,"__android_log_write");
        }

        LOGD("enter hook 32!!!");
    }
}
