package com.van.gba;

public class Cheat
{
	public String mDescription = "";
	public String mCode = "";
	public boolean mEnable = true;
	public Cheat(String description, String code, boolean enable)
	{
		mDescription = description;
		mCode = code;
		mEnable = enable;
	}
}