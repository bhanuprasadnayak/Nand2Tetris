    import sys
import re

# Predefined symbols
SYMBOLS = {
    "SP":0, "LCL":1, "ARG":2, "THIS":3, "THAT":4,
    "R0":0, "R1":1, "R2":2, "R3":3, "R4":4, "R5":5,
    "R6":6, "R7":7, "R8":8, "R9":9, "R10":10, "R11":11,
    "R12":12, "R13":13, "R14":14, "R15":15,
    "SCREEN":16384, "KBD":24576
}

# C-instruction tables
COMP_TABLE = {
    "0":"0101010", "1":"0111111", "-1":"0111010",
    "D":"0001100", "A":"0110000", "M":"1110000",
    "!D":"0001101", "!A":"0110001", "!M":"1110001",
    "-D":"0001111", "-A":"0110011", "-M":"1110011",
    "D+1":"0011111", "A+1":"0110111", "M+1":"1110111",
    "D-1":"0001110", "A-1":"0110010", "M-1":"1110010",
    "D+A":"0000010", "D+M":"1000010", "D-A":"0010011",
    "D-M":"1010011", "A-D":"0000111", "M-D":"1000111",
    "D&A":"0000000", "D&M":"1000000", "D|A":"0010101",
    "D|M":"1010101"
}

DEST_TABLE = {
    None:"000", "M":"001", "D":"010", "MD":"011",
    "A":"100","AM":"101","AD":"110","AMD":"111"
}

JUMP_TABLE = {
    None:"000", "JGT":"001","JEQ":"010","JGE":"011",
    "JLT":"100","JNE":"101","JLE":"110","JMP":"111"
}

class Assembler:
    def __init__(self, asm_file):
        self.asm_file = asm_file
        self.symbols = SYMBOLS.copy()
        self.next_var_address = 16
        self.lines = [line.strip() for line in open(asm_file)]
        self.clean_lines = []
    
    def first_pass(self):
        """Handle labels (e.g., (LOOP)) and remove comments/whitespace"""
        address = 0
        for line in self.lines:
            line = line.split("//")[0].strip()
            if not line:
                continue
            if line.startswith("(") and line.endswith(")"):
                label = line[1:-1]
                self.symbols[label] = address
            else:
                self.clean_lines.append(line)
                address += 1
    
    def parse_c_instruction(self, line):
        if "=" in line:
            dest, rest = line.split("=")
        else:
            dest, rest = None, line
        if ";" in rest:
            comp, jump = rest.split(";")
        else:
            comp, jump = rest, None
        return dest, comp, jump
    
    def a_instruction(self, value):
        try:
            addr = int(value)
        except ValueError:
            if value not in self.symbols:
                self.symbols[value] = self.next_var_address
                self.next_var_address += 1
            addr = self.symbols[value]
        return "0" + format(addr, "015b")
    
    def c_instruction(self, dest, comp, jump):
        comp_bits = COMP_TABLE[comp]
        dest_bits = DEST_TABLE[dest]
        jump_bits = JUMP_TABLE[jump]
        return "111" + comp_bits + dest_bits + jump_bits
    
    def translate(self):
        output = []
        for line in self.clean_lines:
            if line.startswith("@"):
                value = line[1:]
                output.append(self.a_instruction(value))
            else:
                dest, comp, jump = self.parse_c_instruction(line)
                output.append(self.c_instruction(dest, comp, jump))
        return output
    
    def write_hack(self, outfile):
        asm_bin = self.translate()
        with open(outfile, "w") as f:
            f.write("\n".join(asm_bin))
        print(f"Translated {self.asm_file} -> {outfile}")


if __name__ == "__main__":
    asmfile = sys.argv[1]
    outfile = asmfile.replace(".asm", ".hack")
    assembler = Assembler(asmfile)
    assembler.first_pass()
    assembler.write_hack(outfile)

