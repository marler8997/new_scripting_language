module nsl.scriptfile;

import mar.flag;
import mar.expect : ExpectMixin, ErrorCase;
import mar.sentinel : SentinelString;
import mar.mmap : MemoryMap;
import mar.file : FileD;

import nsl.log;

mixin ExpectMixin!("OpenScriptFileResult", ScriptFile,
  ErrorCase!("openFailed", "open file failed, returned %", ptrdiff_t),
  ErrorCase!("mmapFailed", "mmap file failed, returned %", ptrdiff_t)
);

struct ScriptFile
{
    SentinelString filename;
    FileD handle;
    MemoryMap mmap;
    //string text;
    //AstNode[] nodes;

    static OpenScriptFileResult open(SentinelString filename)
    {
        import mar.mmap : createMemoryMap;
        import mar.file : open, OpenFlags, OpenAccess, close;

        ScriptFile file = ScriptFile(filename);
        file.handle = open(filename.ptr, OpenFlags(OpenAccess.readOnly));
        if (!file.handle.isValid)
	    return OpenScriptFileResult.openFailed(file.handle.numval);
	file.mmap = createMemoryMap(null, 0, No.writeable, file.handle, 0);
        if (file.mmap.failed)
        {
	    close(file.handle);
	    return OpenScriptFileResult.mmapFailed(file.mmap.numval);
        }
	return OpenScriptFileResult.success(file);
    }
};
