import re
from abc import ABC
from enum import Enum
from typing import List, Optional


class TokenPosition:
    file: str
    line: int
    column: int

    def __init__(self, file: str, line: int, column: int):
        self.file = file
        self.line = line
        self.column = column


class Token(ABC):
    text: str
    position: TokenPosition

    def __init__(self, text: str, position: TokenPosition):
        self.text = text
        self.position = position


class BracketType(Enum):
    opening_curly = '{'
    closing_curly = '}'
    opening_paren = '('
    closing_paren = ')'
    opening_square = '['
    closing_square = ']'


class Bracket(Token):
    type: BracketType
    opening: bool

    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)
        self.type = BracketType(text)
        self.opening = text in ('{', '(', '[')


class Comma(Token):
    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)


class Identifier(Token):
    value: str

    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)
        self.value = text


class StringLiteral(Token):
    value: str

    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)
        self.value = text


class IntegerLiteral(Token):
    value: int

    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)
        self.value = int(text)


class FloatLiteral(Token):
    value: float

    def __init__(self, text: str, position: TokenPosition):
        super().__init__(text, position)
        self.value = float(text)


# def tokenize(text: str) -> List[Token]:
#     patterns = [
#         (r'\{|}|\(|\)|\[|\]', lambda text: Bracket(text, position)),
#         (r',', lambda text: Comma(text, position)),
#         (r'[a-zA-Z_][a-zA-Z0-9_]*', lambda text: Identifier(text, position)),
#         (r'"(?:[^"\\]|\\.)*"', lambda text: StringLiteral(text, position)),
#         (r'\d+', lambda text: IntegerLiteral(text, position)),
#         (r'\d+(\.\d*)?', lambda text: FloatLiteral(text, position))
#     ]


class TokenizerError(Exception):
    def __init__(self, message: str, position: TokenPosition):
        self.message = message
        self.position = position
        super().__init__(f"{position.file}:{position.line}:{position.column}: {message}")


class Tokenizer:
    def __init__(self, text: str, filename: str = "<input>"):
        self.text = text
        self.filename = filename
        self.source_index = 0
        self.line = 1
        self.column = 1
        self.tokens: List[Token] = []

    def current_position(self) -> TokenPosition:
        return TokenPosition(self.filename, self.line, self.column)

    def peek(self, offset: int = 0) -> Optional[str]:
        index = self.source_index + offset
        if index < len(self.text):
            return self.text[index]
        return None

    def advance(self) -> Optional[str]:
        if self.source_index >= len(self.text):
            return None
        char = self.text[self.source_index]
        self.source_index += 1
        if char == '\n':
            self.line += 1
            self.column = 1
        else:
            self.column += 1
        return char

    def skip_whitespace(self):
        while self.peek() and self.peek() in ' \t\n\r':
            self.advance()

    def match_bracket(self) -> Optional[Token]:
        char = self.peek()
        if char in '{}()[]':
            position = self.current_position()
            self.advance()
            return Bracket(char, position)
        return None

    def match_comma(self) -> Optional[Token]:
        if self.peek() == ',':
            position = self.current_position()
            self.advance()
            return Comma(',', position)
        return None

    def match_identifier(self) -> Optional[Token]:
        char = self.peek()
        if char and (char.isalpha() or char == '_'):
            position = self.current_position()
            text = ''
            while self.peek() and (self.peek().isalnum() or self.peek() == '_'):
                text += self.advance()
            return Identifier(text, position)
        return None

    def match_string_literal(self) -> Optional[Token]:
        if self.peek() != '"':
            return None
        position = self.current_position()
        text = ''
        text += self.advance()  # Opening quote
        while True:
            char = self.peek()
            if char is None:
                raise TokenizerError("Unterminated string literal", position)
            if char == '"':
                text += self.advance()
                break
            if char == '\\':
                text += self.advance()
                escaped = self.peek()
                if escaped is None:
                    raise TokenizerError("Unterminated string literal", position)
                text += self.advance()
            else:
                text += self.advance()
        return StringLiteral(text, position)

    def match_number(self) -> Optional[Token]:
        char = self.peek()
        if not (char and char.isdigit()):
            return None
        position = self.current_position()
        text = ''
        while self.peek() and self.peek().isdigit():
            text += self.advance()
        # Check for float
        if self.peek() == '.' and self.peek(1) and self.peek(1).isdigit():
            text += self.advance()  # Add '.'
            while self.peek() and self.peek().isdigit():
                text += self.advance()
            return FloatLiteral(text, position)
        return IntegerLiteral(text, position)

    def tokenize_all(self) -> List[Token]:
        while self.source_index < len(self.text):
            self.skip_whitespace()
            if self.source_index >= len(self.text):
                break

            position = self.current_position()
            token = (self.match_bracket() or
                     self.match_comma() or
                     self.match_string_literal() or
                     self.match_number() or
                     self.match_identifier())

            if token:
                self.tokens.append(token)
            else:
                char = self.peek()
                raise TokenizerError(f"Unexpected character: '{char}'", position)

        return self.tokens


def tokenize(text: str, filename: str = "<input>") -> List[Token]:
    tokenizer = Tokenizer(text, filename)
    return tokenizer.tokenize_all()
