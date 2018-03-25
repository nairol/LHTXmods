module patchFW;

import std.file, std.stdio, std.format, std.ascii, std.conv, std.string, std.array, std.algorithm;

void main( string[] args )
{
	if( args.length < 4 )
	{
		writeln("Usage: patchFW <SourceBinaryFile> <PatchFile> <DestinationBinaryFile>");
		writeln(" PatchFile must be in Verilog Memory List format");
		return;
	}

	auto orig  = File( args[1] );
	auto patch = File( args[2] );
	
	uint address = 0;
	
	auto obuf = orig.rawRead(new ubyte[0x20000]);
	
	foreach(line; patch.byLine(KeepTerminator.no, std.ascii.newline))
	{
		if( line.startsWith('@') )
		{
			auto adrStr = line[1..$];
			address = adrStr.parse!uint(16);
			
			if(address > 0x20000)
			{
				writeln("Done patching the flash.");
				break;
			}
			
			writefln!"Patching at 0x%08X"(address);
			continue;
		}
		
		auto arr = line.split;
		
		if( address + arr.length > obuf.length )
		{
			obuf.length = address + arr.length;
		}
		
		foreach( size_t i, chr; arr)
		{
			obuf[address+i] = chr.parse!ubyte(16);
		}
		address += arr.length;
	}
	
	writefln!"Final size: %08X"(obuf.length);
	auto newFile = File( args[3], "wb");
	newFile.rawWrite(obuf);
}