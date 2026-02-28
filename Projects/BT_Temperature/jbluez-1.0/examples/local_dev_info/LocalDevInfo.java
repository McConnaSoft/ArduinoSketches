/*
   LocalDevInfo.java - demo application to illustrate the reading of information
   about a local Bluetooth device.

   Copyright (c) 2002 The Appliance Studio Limited.
   Written by Edward Kay <ed.kay@appliancestudio.com>
   http://www.appliancestudio.com

   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License version 2 as published by the
   Free Software Foundation.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.

   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
   OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS SOFTWARE IS
   DISCLAIMED.

   ---

   This little application demonstrates how to get information about a local
   Bluetooth device.

   Note that before running this application, ensure the following:
   * The file 'jbluez.jar' is included in your classpath;
   * The location of 'libjbluez.so' is present in the LD_LIBRARY_PATH
     environment variable.
   * The local Bluetooth device, as represented by the hciDevID field
     is correctly configured and is "up".

   More information on the method parameters and objects returned can be
   found in the Javadoc API documentation.

   $Id:$
*/

import com.appliancestudio.jbluez.*;

public class LocalDevInfo 
{
	private BlueZ bz;

	// Local HCI device ID. This is the same as that reported by the
	// hciconfig tool provided by BlueZ.
	private int hciDevID = 0;

	// Device descriptor. This is the reference to an open HCI device.
	private int dd;

	public static void main(String[] args) 
	{
		LocalDevInfo di = new LocalDevInfo();
		di.getInfo();
	}

	public void getInfo()
	{
		bz = new BlueZ();
		String local_name;
		BTAddress local_addr;
		HCIDeviceInfo info;
		HCIVersion version;

		try
		{
			dd = bz.hciOpenDevice(hciDevID);

			local_name = bz.hciLocalName(dd);
			System.out.println("Device Name: " + local_name);

			local_addr = bz.hciDevBTAddress(hciDevID);
			System.out.println("Device Address: " + local_addr.toString());

			info = bz.hciDevInfo(hciDevID);
			System.out.println("Device Info:\n");
			System.out.println(info.toString());

			version = bz.hciReadLocalVersion(dd, 10000);
			System.out.println("Device Version:\n");
			System.out.println(version.toString());

			bz.hciCloseDevice(dd);
		}
		catch (BlueZException bze)
		{
			System.out.println(bze.getMessage());
			bz.hciCloseDevice(dd);
		}
	}
}
