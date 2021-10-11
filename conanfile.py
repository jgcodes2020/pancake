from conans import ConanFile, CMake, tools
import shutil



class PancakeConan(ConanFile):
    name = "pancake"
    license = "MPL-2.0"
    author = "jgcodes2020 (jackyguo579@gmail.com)"
    url = "https://github.com/jgcodes2020/pancake"
    description = "A library to interface with Wafel's libsm64"
    topics = ("analysis", "sm64", "cpp17")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {
        "shared": False, 
        "fPIC": True
    }
    generators = "cmake"
    exports_sources = "src/*", "scripts/*", "conan_recipes/*", "cmake/*", "CMakeLists.txt", "pancake_metadata.json.in"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
    def _cmake_config(self):
        cmake = CMake(self)
        # Use ninja if available, because it's fast
        if shutil.which("ninja") is not None:
            cmake.generator = "Ninja"
        
        cmake.configure()
        return cmake
            
    def build(self):
        cmake = self._cmake_config()
        cmake.build()
        
    def requirements(self):
        self.requires("libdwarf/0.1.1@jgcodes2020/stable")
        self.requires("LIEF/0.11.5@jgcodes2020/stable")
        self.requires("nlohmann_json/[>=3.9.1]")
        self.requires("ms-gsl/[>=3.1.0]")

    def package(self):
        cmake = self._cmake_config()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["pancake"]
