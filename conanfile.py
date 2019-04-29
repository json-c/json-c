from conans import ConanFile, AutoToolsBuildEnvironment, tools

class JsonclibConan(ConanFile):
    name = "json-c"
    version = "0.1"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of jsonclib here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = { "shared": [True, False] }
    default_options = "shared=False"
    generators = "make"

    def build(self):
        autotools = AutoToolsBuildEnvironment(self)
        self.run("cd .. && autoreconf -fsi ")
        autotools.configure(configure_dir="..",args=["--prefix=${PWD}"])
        autotools.make()
        autotools.install()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*", dst="lib", src="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = [ "json-c" ]
