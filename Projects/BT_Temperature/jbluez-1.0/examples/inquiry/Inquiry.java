/*
   Inquiry.java - demo application to illustrate an HCI inquiry.

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

   This simple application performs an HCI inquiry, which discovers any
   remote Bluetooth devices. The hciInquiry method of the BlueZ instance
   does this and returns the result as an InquiryInfo object. The number
   of devices found is then printed to the standard out, along with the
   Bluetooth device addresses.

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
import java.util.*;

public class Inquiry 
{
	private BlueZ bz;
	// Local HCI device ID. This is the same as that reported by the
	// hciconfig tool provided by BlueZ.
	private int hciDevID = 0;

	public static void main(String[] args) 
	{
		Inquiry i = new Inquiry();
		i.inquire();
	}

	public void inquire()
	{
		bz = new BlueZ();
		InquiryInfo info;

		try
		{
			info = bz.hciInquiry(hciDevID);
			printResults(info);
		}
		catch (BlueZException bze)
		{
			System.out.println(bze.getMessage());
		}

	}

	public void printResults(InquiryInfo info)
	{
		Vector devices = info.devices();

		System.out.println(devices.size() + " devices found.\n");

		for (Enumeration e = devices.elements(); e.hasMoreElements(); )
		{
			InquiryInfoDevice dev = (InquiryInfoDevice) e.nextElement();
			BTAddress bdaddr = dev.bdaddr;
			System.out.println("\t" + bdaddr.toString());
		}
	}
}
