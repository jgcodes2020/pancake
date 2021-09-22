from conans import ConanFile, CMake
from conans.tools import SystemPackageTool, os_info
import subprocess as subp
from os import path
import shutil
from typing import Dict


class LibdwarfConan(ConanFile):

    name = "libdwarf"
    version = "0.1.1"
    license = "LGPL-2.1-only"
    author = "David Anderson (libdwarf-list@linuxmail.org)"
    url = "https://github.com/davea42/libdwarf-code"
    homepage = "https://www.prevanders.net/dwarf.html"
    description = "A consumer library interface to the DWARF debugging format"
    topics = ("conan", "debugging", "dwarf", "dwarf2", "libdwarf")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    
    exports = "patches/*.diff"

    @property
    def _git_srcdir(self):
        return path.join(self.source_folder, "libdwarf-code")
    
    @property
    def _cmake_builddir(self):
        return path.join(self.build_folder, "build")

    def configure(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd
    
    def requirements(self):
        self.requires("zlib/1.2.11")
            
    def _install_system_pkg(self, names: Dict):
        pm = SystemPackageTool()
        if os_info.is_linux:
            if os_info.linux_distro in ["ubuntu", "debian", "linuxmint"]:
                pm.install(names["apt"])
            elif os_info.linux_distro in ["fedora", "centos", "rhel"]:
                pm.install(names["dnf"])
            elif os_info.linux_distro in ["arch"] and names["pacman"] is not None:
                pm.install(names["pacman"])
            elif os_info.linux_distro in ["opensuse", "sles"] and names["zypper"] is not None:
                pm.install(names["zypper"])
        elif os_info.is_macos:
            pm.install(names["brew"])

    def source(self):
        try:
            shutil.rmtree(self._git_srcdir)
        except:
            pass
        self.run(
            "git clone -b libdwarf-0.1.1 https://github.com/davea42/libdwarf-code.git")
        self.run(
            f"git apply {path.join(self.recipe_folder, self.conan_data['patches'][self.version])}",
            cwd=path.join(self.source_folder, "libdwarf-code"))

    def build(self):
        cmake = CMake(self)
        
        # Use ninja if available, because it's fast
        if shutil.which("ninja") is not None:
            cmake.generator = "Ninja"
        
        cmake.configure(source_dir="libdwarf-code")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        libdwarf_dir = path.join(self.source_folder, "libdwarf-code",
                                 self.conan_data["package"][self.version]["libdwarf_dir"])
        # Use shutil.copy to move from source
        self.copy("*dwarf.h", src=libdwarf_dir, dst="include/libdwarf")
        self.copy("*.lib", src="lib", dst="lib")
        self.copy("lib*.a", src="lib", dst="lib")

    def package_info(self):
        self.cpp_info.libs = ["dwarf"]
