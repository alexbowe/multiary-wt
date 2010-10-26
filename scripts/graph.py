#!/usr/bin/python

import sys, re
from string import Template

templates = { 'outfile' : Template('$type.$size'),
              'outrow' : Template('$arity $time $wt_size $tab_size') }

row_regexes = [ r'(?P<structure>[\w-]+)',
                r'(?P<arity>\d+)',
                r'(?P<type>[\w.]+)',
                r'(?P<size>\d+MB)',
                r'(?P<time>\d+\.\d+)',
                r'(?P<wt_size>\d+)',
                r'(?P<tab_size>\d+)' ]
row_regex = re.compile('\s'.join(row_regexes))

def usage():
    sys.stderr.write("Usage: python graph.py <file>\n")

def outName(d):
    return templates['outfile'].substitute(d)

def outRow(d):
    return templates['outrow'].substitute(d)

def getRowData(row):
    m = row_regex.match(row)
    return m.groupdict()

def processFile(f):
    for line in f:
        yield getRowData(line)

def getData(f):
    data = { }
    
    for r in processFile(f):
        out = outName(r)
        if out not in data:
            data[out] = dict()
        
        struct = r['structure']
        if struct not in data[out]:
            data[out][struct] = []
        
        data[out][struct].append(r)
    
    return data

def writeData(data):
    for filename in data:
        with open(filename, 'w') as f:
            # keep all structures together
            for structure in data[filename]:
                rows = data[filename][structure]
                output = [outRow(r) for r in rows]
                f.write('# {0}\n'.format(structure))
                for row in output:
                    f.write(row + '\n')
                f.write('\n\n')

if __name__ == '__main__':
    if len(sys.argv) is not 2:
        usage()
        exit()
    try:
        filename = sys.argv[1]
        
        with open(filename, 'r') as f:
            data = getData(f)
        
        writeData(data)
    
    except Exception as e:
        sys.stderr.write(str(e) + '\n')
        usage()
