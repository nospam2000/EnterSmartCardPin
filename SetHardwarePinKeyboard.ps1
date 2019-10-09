// Copyright (c) 2019 by michael@5dot1.de
// License: Apache Version 2.0

$pin = Read-Host 'Enter Your Pin:' -AsSecureString
$pinUnsecure = [Runtime.InteropServices.Marshal]::PtrToStringAuto([Runtime.InteropServices.Marshal]::SecureStringToBSTR($pin))

$port= new-Object System.IO.Ports.SerialPort COM9,115200,None,8,one
$port.open()
$port.WriteLine("^" + $pinUnsecure + "`n")
$port.Close()
