in_file_name = 'russian_english.dic'
out_file_name = 'russian_english_sorted.dic'

with open(in_file_name, 'r') as in_file:
    with open(out_file_name, 'w') as out_file:
        lines = set()
        for i, line in enumerate(in_file):  # add unique lines
            if i == 0:
                continue
            lines.add(line)

        '''
        lines_ = set()  # remove lines that have lowercase counterparts
        for line in lines:
            if line.lower() == line or line.lower() not in lines:
                lines_.add(line)
        lines = lines_
        '''

        sorted_lines = sorted(lines)

        out_file.write(str(len(lines)) + '\n')
        for line in sorted_lines:
            out_file.write(line)
