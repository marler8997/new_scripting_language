module nsl.log;

pragma(inline)
void logError(T...)(T args)
{
    import mar.io : stderr;
    stderr.writeln("Error: ", args);
}
