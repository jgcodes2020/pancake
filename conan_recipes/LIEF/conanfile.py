from conans import ConanFile, CMake, tools
from os import path

class LIEFConan(ConanFile):
    name = "LIEF"
    version = "0.11.5"
    license = "Apache-2.0"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of LIEF here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False], 
        "fPIC": [True, False],
        "use_json": [True, False],
        "use_frozen": [True, False],
        "c_api_support": [True, False],
        "pe_support": [True, False],
        "elf_support": [True, False],
        "macho_support": [True, False],
        "dex_support": [True, False],
        "art_support": [True, False],
        "oat_support": [True, False],
        "vdex_support": [True, False]
    }
    default_options = {
        "shared": False, 
        "fPIC": True,
        "use_json": True,
        "use_frozen": True,
        "c_api_support": True,
        "pe_support": True,
        "elf_support": True,
        "macho_support": True,
        "dex_support": True,
        "art_support": True,
        "oat_support": True,
        "vdex_support": True
    }
    generators = "cmake"
    
    exports = "patches/*.diff"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def source(self):
        self.run("git clone -b 0.11.5 https://github.com/lief-project/LIEF.git")
        self.run(
            f"git apply {path.join(self.recipe_folder, self.conan_data['patches'][self.version])}",
            cwd=path.join(self.source_folder, "LIEF"))
        
        

    def requirements(self):
        self.requires("boost/1.75.0")
        self.requires("utfcpp/3.1.2")
        self.requires("mbedtls/2.25.0@jgcodes2020/stable")
        self.requires("spdlog/1.8.2")
        if self.options.use_json:
            self.requires("nlohmann_json/3.9.1")
        if self.options.use_frozen:
            self.requires("frozen/1.0.1")
        
    def _cmake_config(self):
        cmake = CMake(self)
        cmake.configure(source_folder="LIEF", defs={
            "LIEF_USE_JSON": self.options.use_json,
            "LIEF_FROZEN_DISABLED": not self.options.use_frozen,
            "LIEF_C_API": self.options.c_api_support,
            "LIEF_ELF": self.options.elf_support,
            "LIEF_PE": self.options.pe_support,
            "LIEF_MACHO": self.options.macho_support,
            "LIEF_DEX": self.options.dex_support,
            "LIEF_ART": self.options.art_support,
            "LIEF_OAT": self.options.oat_support,
            "LIEF_VDEX": self.options.vdex_support,
            "LIEF_EXAMPLES": False,
            "LIEF_PYTHON_API": False,
            "LIEF_LOGGING": False,
            "LIEF_TEST": False,
            "LIEF_DOC": False,
        })
        return cmake
        

    def build(self):
        cmake = self._cmake_config()
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        cmake = self._cmake_config()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["LIEF"]
