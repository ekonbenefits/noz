// Learn more about F# at http://fsharp.org

open System
open System.IO

[<EntryPoint>]
let main argv =
    for file in argv do
        let input = File.ReadAllBytes(file);
        let output = input |> Array.filter(fun b -> b <> 0x1Duy)
        File.WriteAllBytes(file, output)

    0 // return an integer exit code
