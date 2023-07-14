from conan import ConanFile

from conan.tools.cmake import cmake_layout
from conan.tools.env import Environment

class MCTS_4Wins(ConanFile):
    name = "mypkg"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("boost/[>=1.0.0]")
        self.requires("qt/5.15.9")

    def configure(self):
        self.options["boost"].shared = True
        self.options["qt"].shared = True
        self.options["qt"].opengl = "desktop"
        self.options["qt"].GUI = True

    def build_requirements(self):
        self.tool_requires("cmake/3.26.4")
