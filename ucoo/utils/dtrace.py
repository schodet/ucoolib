"""GDB plugin to dump trace from a Trace object."""

class DTrace(gdb.Command):
    def __init__(self):
        gdb.Command.__init__(self, "dtrace", gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL, True)
    
    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)
        if len(args) != 1:
            print 'Usage: dtrace <object>'
            return

        trace = gdb.parse_and_eval(args[0])
        args_nb = trace['args_nb'];
        entries_nb = trace['entries_nb'];
        index = trace['index'];
        entries = trace['entries'];
        
        if entries[index]['str']:
            r = range(index, entries_nb) + range(index)
        else:
            r = range(index)
        
        for i in r:
            entry = entries[i]
            if not entry['str']:
                break
            s = entry['str'].string()
            sargs = s.count('%') - 2 * s.count('%%')
            args = tuple(entry['args'][i] for i in xrange(sargs))
            try:
                s = s % args
            except:
                pass
            if s.startswith('<'):
                n = int(s[1:].partition('>')[0])
                s = ' ' * (n * 10) + s
            print s

DTrace()
