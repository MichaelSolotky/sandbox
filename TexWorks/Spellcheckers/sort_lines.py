in_file_name = 'russian_english.dic'
out_file_name = 'russian_english_sorted.dic'

with open(in_file_name, 'r') as in_file:
    with open(out_file_name, 'w') as out_file:
        strings = []
        for i, line in enumerate(in_file):
            if i == 0:
                continue
            strings.append(line)
        sorted_strings = sorted(strings)

        out_file.write(str(i) + '\n')
        for string in sorted_strings:
            out_file.write(string)
