package = "lua-conio"
version = "1.0-0"
source = {
    url = "git+https://github.com/joe7575/conio.git"
}

description = {
    summary = "Turbo C conio.h clone for windows and linux",
    detailed = [[
	This library implements (parts) the of old Turbo C "conio.h".
        See header file for suported functions.
        To avoid name conflicts a prefix "c_" was added into the original functions.
    ]],
    homepage = "https://github.com/joe7575/conio",
    license = "MIT"
}

dependencies = {
   "lua == 5.1"
}

build = {
    type = "builtin",
    modules = {
        conio = {
            "conio.c",
            "coniolua.c",
        },
    }
}
