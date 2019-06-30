#!/usr/bin/env python

"""
Mocking utility for Narwhal (https://github.com/vberlier/narwhal)

MIT License

Copyright (c) 2019 Valentin Berlier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

from __future__ import print_function, unicode_literals

import os
import sys
import re
from collections import namedtuple, defaultdict
from argparse import ArgumentParser, FileType
from textwrap import dedent


FunctionDeclaration = namedtuple(
    "FunctionDeclaration", ["name", "parameters", "return_type", "source"]
)


SourceLocation = namedtuple("SourceLocation", ["filename", "line"])


class IncludeDirective(namedtuple("IncludeDirective", ["path", "system"])):
    @classmethod
    def from_source_context(cls, source_context):
        if not source_context:
            return None

        for source in source_context:
            if not os.path.isabs(source.filename):
                continue

            dirname, basename = os.path.split(source.filename)
            fullname = basename

            while True:
                dirname, basename = os.path.split(dirname)

                if "include" in basename.lower():
                    return cls(fullname, True)

                if not basename:
                    break

                fullname = os.path.join(basename, fullname)

        return cls(os.path.abspath(source_context[-1].filename), False)


class Token(namedtuple("Token", ["type", "value"])):
    def is_punctuation(self, value):
        return self.type == "PUNCTUATION" and self.value == value

    @property
    def is_prefix(self):
        return self.type == "KEYWORD" and self.value in (
            "typedef",
            "extern",
            "static",
            "auto",
            "register",
            "__extension__",
        )


class FunctionDeclarationParser(object):
    linemarker = re.compile(r'^# (\d+) "((?:\\.|[^\\"])*)"((?: [1234])*)$')

    tokens = (
        ("LINEMARKER", r"^#.*$"),
        (
            "KEYWORD",
            "\\b(?:auto|break|case|char|const|continue|default|do|double|else|enum|extern|float"
            "|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch"
            "|typedef|union|unsigned|void|volatile|while|__extension__)\\b",
        ),
        ("IDENTIFIER", r"\b[a-zA-Z_](?:[a-zA-Z_0-9])*\b"),
        ("CHARACTER", r"L?'(?:\\.|[^\\'])+'"),
        ("STRING", r'L?"(?:\\.|[^\\"])*"'),
        ("INTEGER", r"(?:0[xX][a-fA-F0-9]+|[0-9]+)[uUlL]*"),
        (
            "FLOAT",
            r"(?:[0-9]+[Ee][+-]?[0-9]+|[0-9]*\.[0-9]+(?:[Ee][+-]?[0-9]+)?|[0-9]+\.[0-9]*(?:[Ee][+-]?[0-9]+)?)[fFlL]?",
        ),
        (
            "PUNCTUATION",
            r"\.\.\.|>>=|<<=|\+=|-=|\*=|/=|%=|&=|\^=|\|=|>>|<<|\+\+|--|->|&&|\|\||<=|>=|"
            r"==|!=|;|\{|\}|,|:|=|\(|\)|\[|\]|\.|&|!|~|-|\+|\*|/|%|<|>|\^|\||\?",
        ),
        ("SPACE", r"[ \t\v\n\f]*"),
        ("IGNORE", r".+?"),
    )

    ignored_tokens = "SPACE", "IGNORE"

    regex = re.compile(
        "|".join("(?P<" + token + ">" + pattern + ")" for token, pattern in tokens),
        flags=re.MULTILINE,
    )

    def __init__(self, string, filter=None):
        self.string = string
        self.filter = set(filter) if filter is not None else None
        self.token_stream = self.tokenize(string)
        self.current = None

        self.bracket_stack = []
        self.source_context = []

    @classmethod
    def tokenize(cls, string):
        for match in cls.regex.finditer(string):
            if match.lastgroup not in cls.ignored_tokens:
                yield Token(match.lastgroup, match.group().strip())

    def __iter__(self):
        while self.next():
            function = self.parse_function_declaration()

            if function is not None:
                yield function

            if self.filter is not None and not self.filter:
                break

            while self.current and not (
                self.current.type == "PUNCTUATION"
                and self.current.value in (";", "}")
                and not self.bracket_stack
            ):
                self.next()

    def next(self):
        self.current = next(self.token_stream, None)

        if not self.current:
            return None

        if self.current.type == "PUNCTUATION":
            if self.current.value in "({[":
                self.bracket_stack.append(")}]"["({[".index(self.current.value)])
            elif self.bracket_stack and self.current.value == self.bracket_stack[-1]:
                self.bracket_stack.pop()

        elif self.current.type == "LINEMARKER":
            line, filename, flags = self.linemarker.match(self.current.value).groups()
            context = SourceLocation(filename, int(line))

            if "1" in flags:
                self.source_context.append(context)
            elif "2" in flags:
                self.source_context.pop()

            try:
                self.source_context[-1] = context
            except IndexError:
                self.source_context.append(context)

            self.next()

        return self.current

    def parse_function_declaration(self):
        if self.bracket_stack:
            return None

        while self.current and self.current.is_prefix:
            self.next()

        return_type = []

        while self.current and not self.current.is_punctuation("("):
            if self.current.is_punctuation(";"):
                return None

            return_type.append(self.current.value)
            self.next()

        if not return_type:
            return None

        func_name = return_type.pop()

        self.next()

        if self.filter is not None:
            if func_name in self.filter:
                self.filter.remove(func_name)
            else:
                return None

        parameters = []

        while self.current and not self.current.is_punctuation(")"):
            if self.current.is_punctuation(";"):
                break

            param = self.parse_parameter("arg" + str(len(parameters) + 1))

            if param:
                parameters.append(param)

            if self.current and self.current.value == ",":
                self.next()

        self.next()

        return FunctionDeclaration(
            func_name,
            parameters,
            return_type,
            IncludeDirective.from_source_context(self.source_context),
        )

    def parse_parameter(self, param_name):
        tokens = []

        while self.current and not (
            self.current.type == "PUNCTUATION"
            and (
                self.current.value == ","
                and len(self.bracket_stack) == 1
                or self.current.value == ")"
                and not self.bracket_stack
            )
        ):
            if self.current.is_punctuation(";"):
                break

            tokens.append(self.current)
            self.next()

        if not tokens:
            return

        parameter = []

        reversed_tokens = iter(reversed(tokens))

        for token in reversed_tokens:
            if token.type == "IDENTIFIER":
                parameter = [param_name] + parameter
                break
            parameter = [token.value] + parameter

        for token in reversed_tokens:
            parameter = [token.value] + parameter

        return param_name, self.format_type(" ".join(parameter))

    @staticmethod
    def format_type(string):
        return re.sub(r" (\[) |([(*]) | ([\])])", r"\1\2\3", string)


def template(string):
    return dedent(
        re.sub(r"(\{(?![a-zA-Z_])|(?<![a-zA-Z_])\})", r"\1\1", string)
    ).strip()


HEADER_TEMPLATE = (
    template(
        """
    #ifndef {guard_name}
    #define {guard_name}

    {declarations}

    #endif
    """
    )
    + "\n"
)


class CodeGenerator(object):
    DECL_BEGIN = "/*\nNARMOCK_DECLARATIONS_BEGIN\n*/"
    DECL_END = "/*\nNARMOCK_DECLARATIONS_END\n*/"

    DECL_MARKER = "NARMOCK_DECLARATION"
    FLAG_MARKER = "NARMOCK_LINKER_FLAGS"
    IMPL_MARKER = "NARMOCK_IMPLEMENTATION"

    file_template = (
        template(
            """
            {DECL_BEGIN}

            {includes}

            {declarations}

            {DECL_END}

            {implementations}
            """
        )
        + "\n"
    )

    mock_declaration = template(
        """
        /*
        {decl_marker} {func_name}
        {flag_marker} {linker_flags}
        */

        typedef struct {parameters_struct}
        {
            {function_parameter_fields}
        } {parameters_struct};

        typedef struct {state_type} {state_type};

        struct {state_type}
        {
            {state_type} *(*mock_return)({return_value_param});
            {state_type} *(*mock_implementation)({return_type} (*implementation)({function_parameters}));
            {state_type} *(*disable_mock)(void);
        };

        {mock_aliases}
        """
    )

    mock_implementation = template(
        """
        /*
        {impl_marker} {func_name}
        */

        {return_type} {real_func}({function_parameters});

        typedef struct {private_state_type} {private_state_type};

        struct {private_state_type}
        {
            {state_type} public;

            int state;
            {return_field}
            {return_type} (*implementation)({function_parameters});
        };

        {state_type} *_narmock_function_mock_return_{func_name}({return_value_param});
        {state_type} *_narmock_function_mock_implementation_{func_name}({return_type} (*implementation)({function_parameters}));
        {state_type} *_narmock_function_disable_mock_{func_name}();

        {private_state_type} {state_name} =
        {
            .public = {
                .mock_return = _narmock_function_mock_return_{func_name},
                .mock_implementation = _narmock_function_mock_implementation_{func_name},
                .disable_mock = _narmock_function_disable_mock_{func_name}
            },

            .state = 0
        };

        {return_type} {wrapped_func}({function_parameters})
        {
            switch ({state_name}.state)
            {
                case 1:
                    return {state_return_value};
                case 2:
                    return {state_name}.implementation({parameter_arguments});
                default:
                    return {real_func}({parameter_arguments});
            }
        }

        {state_type} *_narmock_function_mock_return_{func_name}({return_value_param})
        {
            {state_name}.state = 1;
            {update_state_return_value}

            return &{state_name}.public;
        }

        {state_type} *_narmock_function_mock_implementation_{func_name}({return_type} (*implementation)({function_parameters}))
        {
            {state_name}.state = 2;
            {state_name}.implementation = implementation;

            return &{state_name}.public;
        }

        {state_type} *_narmock_function_disable_mock_{func_name}()
        {
            {state_name}.state = 0;

            return &{state_name}.public;
        }

        {mock_aliases}
        """
    )

    def __init__(self, filename):
        self.filename = os.path.abspath(filename)
        self.directory = os.path.dirname(self.filename)
        self.system_includes = set()
        self.local_includes = set()
        self.declarations = []
        self.implementations = []

    def generate_implementation_file(self):
        includes = (
            "\n".join(
                "#include <" + path + ">" for path in sorted(self.system_includes)
            )
            + ("\n\n" if self.system_includes and self.local_includes else "")
            + "\n".join(
                '#include "' + os.path.relpath(path, self.directory) + '"'
                for path in sorted(self.local_includes)
            )
        )

        return self.file_template.format(
            includes=includes,
            declarations="\n\n".join(self.declarations),
            implementations="\n\n".join(self.implementations),
            DECL_BEGIN=self.DECL_BEGIN,
            DECL_END=self.DECL_END,
        )

    def add_mock(self, aliases, function):
        decl_marker = self.DECL_MARKER
        flag_marker = self.FLAG_MARKER
        impl_marker = self.IMPL_MARKER

        func_name = function.name

        state_name = "_narmock_state_global_" + func_name
        state_type = "_narmock_state_type_" + func_name
        private_state_type = "_narmock_state_private_type_" + func_name

        return_type = " ".join(function.return_type).strip()
        return_value = "" if return_type == "void" else return_type + " return_value"
        return_field = return_value and return_value + ";"
        return_value_param = return_value or "void"
        state_return_value = return_value and state_name + ".return_value"
        update_state_return_value = (
            return_value and state_return_value + " = return_value;"
        )

        function_parameter_fields = "\n    ".join(
            parameter + ";" for _, parameter in function.parameters
        )
        parameters_struct = "_narmock_parameters_" + func_name

        wrapped_func = "__wrap_" + func_name
        real_func = "__real_" + func_name

        function_parameters = ", ".join(
            parameter for _, parameter in function.parameters
        )

        parameter_arguments = ", ".join(name for name, _ in function.parameters)

        linker_flags = " ".join(["-Wl,--wrap=" + func_name])

        mock_aliases = "\n".join(state_type + " *" + alias + "();" for alias in aliases)

        self.declarations.append(self.mock_declaration.format(**locals()))

        mock_aliases = "\n\n".join(
            state_type
            + " *"
            + alias
            + "()\n{\n    return &"
            + state_name
            + ".public;\n}"
            for alias in aliases
        )

        self.implementations.append(self.mock_implementation.format(**locals()))

        if function.source:
            if function.source.system:
                self.system_includes.add(function.source.path)
            else:
                self.local_includes.add(function.source.path)


def collect_mocked_functions(prefixes, expanded_source):
    mock_aliases = defaultdict(set)
    regex = (
        r"(_narmock_state_type_[A-Za-z0-9_]+\s*\*\s*)?\b((?:"
        + "|".join(prefixes)
        + r")([A-Za-z0-9_]+))\s*\(\s*\)"
    )

    for match in re.finditer(regex, expanded_source):
        return_type, alias, mocked_function = match.groups()
        if not return_type:
            mock_aliases[mocked_function].add(alias)

    for function in FunctionDeclarationParser(expanded_source, filter=mock_aliases):
        yield mock_aliases[function.name], function
        del mock_aliases[function.name]

    if mock_aliases:
        for function in mock_aliases:
            print("error:", "'" + function + "' undeclared", file=sys.stderr)
        sys.exit(1)


parser = ArgumentParser(prog="narmock", description="Mocking utility for Narwhal.")

group = parser.add_mutually_exclusive_group(required=True)
group.add_argument("-g", metavar="<file>", help="generate mocks")
group.add_argument("-d", metavar="<file>", help="extract declarations")
group.add_argument("-f", action="store_true", help="output linker flags")

parser.add_argument("-p", metavar="<string>", action="append", help="mock prefix")
parser.add_argument(
    "file",
    metavar="<file>",
    nargs="?",
    type=FileType("r"),
    default=sys.stdin,
    help="expanded code or generated mocks",
)


def main():
    args = parser.parse_args()

    if args.g:
        generator = CodeGenerator(args.g)
        prefixes = ["_narwhal_mock_"] + (args.p or [])

        for aliases, function in collect_mocked_functions(prefixes, args.file.read()):
            generator.add_mock(aliases, function)

        with open(args.g, "w") as implementation:
            implementation.write(generator.generate_implementation_file())

    elif args.d:
        source = args.file.read()

        guard_name = (
            re.sub(
                r"_+",
                "_",
                re.sub(r"[^a-zA-Z0-9]", "_", os.path.normpath(os.path.relpath(args.d))),
            )
            .upper()
            .strip("_")
        )

        try:
            begin = source.index(CodeGenerator.DECL_BEGIN) + len(
                CodeGenerator.DECL_BEGIN
            )
            end = source.index(CodeGenerator.DECL_END)

            if 0 <= begin <= end:
                with open(args.d, "w") as header:
                    header.write(
                        HEADER_TEMPLATE.format(
                            guard_name=guard_name,
                            declarations=source[begin:end].strip(),
                        )
                    )
        except ValueError:
            pass

    elif args.f:
        print(
            " ".join(
                re.findall(
                    r"\b" + CodeGenerator.FLAG_MARKER + r"\s+(.+)", args.file.read()
                )
            ).strip()
        )


if __name__ == "__main__":
    main()
