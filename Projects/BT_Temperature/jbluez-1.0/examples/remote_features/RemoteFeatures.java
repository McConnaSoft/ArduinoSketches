/*
   RemoteFeatures.java - demo application to illustrate the reading of the
   features of a remote Bluetooth device.

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

   This simple application performs reads the features of a remote Bluetooth
   device. This is achieved by calling the hciReadRemoteFeatures method. In
   order to do this, however, the local HCI device must first be opened (see
   hciOpenDevice) and an HCI connection established to the remote device (see
   hciCreateConnection).

   The creation of an HCI connection requires root permissions, so if this
   program is executed without high enough permissions, you will receive a
   BlueZException with the message "Unable to create connection".

   The creation of connections to remote Bluetooth devices without needing root
   permissions is not permitted at the HCI level. Such connections are
   established at higher levels of the protocol stack, usually L2CAP.

   Be aware that the features printed to the standard out are just decimal
   representations of binary feature sets. Mapping these to more user friendly
   strings etc. has not yet been implemented. Please refer to the Bluetooth
   specification document for full details.

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

public class RemoteFeatures 
{
	private BlueZ bz;

	// Local HCI device ID. This is the same as that reported by the
	// hciconfig tool provided by BlueZ.
	private int hciDevID = 0;

	// Device descriptor. This is the reference to an open HCI device.
	private int dd;

	// The handle of an established HCI connection to a remote BT device.
	int handle;

	// Remote Bluetooth device address with which to establish connection.
	private String remote_address = "00:80:98:24:5D:4B";

	public static void main(String[] args) 
	{
		RemoteFeatures rf = new RemoteFeatures();
		rf.readFeatures();
	}

	public void readFeatures()
	{
		bz = new BlueZ();
		HCIDeviceInfo info;
		HCIFeatures features;

		try
		{
			dd = bz.hciOpenDevice(hciDevID);
			handle = bz.hciCreateConnection(dd, remote_address, 8, 0, (short)0, 25000);
			features = bz.hciReadRemoteFeatures(dd, handle, 25000);
			bz.hciDisconnect(dd, handle, (short)13, 10000);
			bz.hciCloseDevice(dd);

			System.out.println("Remote features:\n");
			System.out.println(features.toString() + "\n");
		}
		catch (BlueZException bze)
		{
			System.out.println(bze.getMessage());
			bz.hciCloseDevice(dd);
		}

	}

}
