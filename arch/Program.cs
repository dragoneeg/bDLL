using System;
using System.IO;

namespace arch
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.Title = "C# Arch";

            GetDll32Or64(args[0]);

        }
        private static void GetDll32Or64(string path)
        {
            //var dllPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, @path);
            var result = GetPeArchitecture(@path);

            //Console.WriteLine(path);
            //523 64    267 32
            if (result == 523)
                Console.WriteLine("Arch=x64");
            else if (result == 267)
                Console.WriteLine("Arch=x86");
            else
                Console.WriteLine("Some ERROR!");
        }
        public static ushort GetPeArchitecture(string dllFilePath)
        {
            ushort architecture = 0;

            try
            {
                using (var fStream = new FileStream(dllFilePath, FileMode.Open, FileAccess.Read))
                {
                    using (var bReader = new BinaryReader(fStream))
                    {
                        if (bReader.ReadUInt16() == 23117) //check the MZ signature
                        {
                            fStream.Seek(0x3A, SeekOrigin.Current); //seek to e_lfanew.
                            fStream.Seek(bReader.ReadUInt32(), SeekOrigin.Begin); //seek to the start of the NT header.
                            if (bReader.ReadUInt32() == 17744) //check the PE\0\0 signature.
                            {
                                fStream.Seek(20, SeekOrigin.Current); //seek past the file header,
                                architecture = bReader.ReadUInt16(); //read the magic number of the optional header.
                            }
                        }
                    }
                }
            }
            catch
            {
                // ignored
            }

            // if architecture returns 0, there has been an error.
            return architecture;
        }
    }
}
