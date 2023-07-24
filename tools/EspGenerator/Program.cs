using System.IO.Abstractions;
using Mutagen.Bethesda.Plugins;
using Mutagen.Bethesda.Plugins.Binary.Parameters;
using Mutagen.Bethesda.Skyrim;

var mod = new SkyrimMod(ModKey.FromFileName("skse_plugin_template.esp"), SkyrimRelease.SkyrimSE);
mod.WriteToBinary("./skse_plugin_template.esp", fileSystem: new FileSystem(), param: new BinaryWriteParameters()
{
    ModKey = ModKeyOption.NoCheck
});
