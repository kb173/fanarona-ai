from translate import Translator
translator = Translator(to_lang="de")

import sys

def translate_file(filename):
    with open(filename, "r") as f:
        lines = f.read().splitlines()
        
        for i in range(0, len(lines)):
            if "//" in lines[i]:
                split = lines[i].split("//", 1)
                before = split[0]
                after = split[1]
                
                new_text = before + "// "
                new_text += translator.translate(after)
                
                print(new_text)
                
                lines[i] = new_text

    with open(filename, "w") as f:
        f.write("\n".join(lines))


if len(sys.argv) < 2:
    print("Please specify at least one file to translate!")
else:
    for i in range(1, len(sys.argv)):
        translate_file(sys.argv[i])
