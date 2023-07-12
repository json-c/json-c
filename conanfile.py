import fnmatch
from conans import ConanFile, CMake, tools

class JsonclibConan(ConanFile):
    name = "json-c"
    version = "0.16.99"
    description = "JSON-C - A JSON implementation in C"
    topics = ("conan", "json-c", "json", "encoding", "decoding", "manipulation")
    url = "https://github.com/elear-solutions/json-c"
    license = "MIT"
    generators = "cmake"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False]
    }
    default_options = {
        "shared": False
    }

    def configure(self):
        pass

    def build(self):
        cmake = CMake(self)
        cmake.definitions['BUILD_SHARED_LIBS'] = True
        cmake.definitions['BUILD_STATIC_LIBS'] = True

        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package(self):
        self.copy("*.h", dst="include", src="package/include")
        self.copy("*", dst="lib", src="package/lib")

    def package_info(self):
        self.cpp_info.libs = [ "json-c" ]
