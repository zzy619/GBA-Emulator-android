package com.van.gba;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipException;
import java.util.zip.ZipInputStream;

//import com.google.ads.AdRequest;
//import com.google.ads.AdSize;
//import com.google.ads.AdView;
//import com.van.fceux2.MainActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.DataSetObserver;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.widget.Toast;
import com.van.gba.R;

public class SelectGameActivity extends Activity {
	
	List<String> mFoundFiles = new ArrayList<String>();
	String strFile = "";
	String strAutoSaveFile = "";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		//this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		this.setContentView(R.layout.select_game);
		
//		AdView adView = new AdView(this, AdSize.BANNER, MainActivity.admodId);
//		ViewGroup adsContainer = (ViewGroup)this.findViewById(R.id.adsContainer);
//		adsContainer.addView(adView);
//		AdRequest oAdRequest = new AdRequest();
//		adView.loadAd(oAdRequest);
		
		if(!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED))
		{
			Toast.makeText(this, "This app required a SD card!", Toast.LENGTH_LONG).show();
		}
		
		String strTempPath = Environment.getExternalStorageDirectory().getAbsolutePath();
		strTempPath += "/Android/data/";
		strTempPath += SelectGameActivity.this.getPackageName();
		File dir = new File(strTempPath);
		dir.mkdirs();
		
//		this.startSupportActionMode(new Callback(){
//			@Override
//			public boolean onCreateActionMode(ActionMode arg0, Menu arg1) {
//				return false;
//			}
//			@Override
//			public void onDestroyActionMode(ActionMode arg0) {
//			}
//			@Override
//			public boolean onPrepareActionMode(ActionMode arg0, Menu arg1) {
//				return false;
//			}
//			@Override
//			public boolean onActionItemClicked(ActionMode arg0, MenuItem arg1) {
//				if(arg1.getItemId() == R.id.action_search)
//				{
//					
//				}
//				return false;
//			}});
		
		EditText et = (EditText)findViewById(R.id.editText1);
		et.setOnEditorActionListener(new OnEditorActionListener(){
			@Override
			public boolean onEditorAction(TextView arg0, int arg1, KeyEvent arg2) {
				EditText tv = (EditText)SelectGameActivity.this.findViewById(R.id.editText1);
				String strRom = tv.getText().toString().trim();
				if(strRom.compareTo("") == 0)
				{
					strRom = "best+gba+games";
					Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse("http://www.google.com/search?q=" + strRom));
					SelectGameActivity.this.startActivity(intent);
					return false;
				}
				else
				{
					while(strRom.contains("  "))
					{
						strRom = strRom.replace("  ", " ");
					}
					strRom += " gba";
					strRom = strRom.replace(' ', '+');
					Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse("http://www.google.com/search?q=" + strRom));
					SelectGameActivity.this.startActivity(intent);
				}
				return false;
			}});
		
		final Button searchButton = (Button)this.findViewById(R.id.Search);
		searchButton.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View arg0)
			{
				EditText tv = (EditText)SelectGameActivity.this.findViewById(R.id.editText1);
				String strRom = tv.getText().toString().trim();
				if(strRom.compareTo("") == 0)
				{
					strRom = "best+gba+games";
					Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse("http://www.google.com/search?q=" + strRom));
					SelectGameActivity.this.startActivity(intent);
					return;
				}
				else
				{
					while(strRom.contains("  "))
					{
						strRom = strRom.replace("  ", " ");
					}
					strRom += " gba";
					strRom = strRom.replace(' ', '+');
					Intent intent = new Intent(Intent.ACTION_VIEW).setData(Uri.parse("http://www.google.com/search?q=" + strRom));
					SelectGameActivity.this.startActivity(intent);
				}
			}
		});
		
		final Button oButton = (Button)this.findViewById(R.id.ScanSDCard);
		oButton.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View arg0)
			{
				ScanSDCardTask scanThread = new ScanSDCardTask(SelectGameActivity.this, (ListView)SelectGameActivity.this.findViewById(R.id.ListFiles), oButton);
				scanThread.execute();
			}
		});
		
		ScanSDCardTask scanThread = new ScanSDCardTask(SelectGameActivity.this, (ListView)SelectGameActivity.this.findViewById(R.id.ListFiles), oButton);
		scanThread.execute();
		
		ListView lv = (ListView)SelectGameActivity.this.findViewById(R.id.ListFiles);
		lv.setOnItemClickListener(new OnItemClickListener(){
			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) 
			{
				try
				{
					strFile = arg0.getItemAtPosition(arg2).toString();
					String strFileNameOnly = strFile.substring(strFile.lastIndexOf('/') + 1);
					String strExtension = getExtension(strFile);
					String strName = strFileNameOnly.substring(0, strFileNameOnly.length() - strExtension.length());
					strAutoSaveFile = Environment.getExternalStorageDirectory().getAbsolutePath() + "/Android/data/" + SelectGameActivity.this.getPackageName() + "/" + strName + ".sgm";

					org.libsdl.app.SDLActivity.mRomFile = strFile;
					
//					File autoSaveFile = new File(strAutoSaveFile);
//					if(autoSaveFile.exists())
//					{
//						AlertDialog.Builder builder = new AlertDialog.Builder(SelectGameActivity.this);
//						builder.setTitle(strFileNameOnly);
//						builder.setMessage("Continue from last exit?");
//						builder.setCancelable(true);
//						builder.setPositiveButton("Yes", new DialogInterface.OnClickListener(){
//							@Override
//							public void onClick(DialogInterface dialog, int which) {
//								Intent oIntent = new Intent(SelectGameActivity.this, MainActivity.class);
//								oIntent.putExtra("file", strFile);
//								oIntent.putExtra("autosave", strAutoSaveFile);
//								oIntent.putExtra("resetGame", false);
//								MainActivity.loadAutoSave = true;
//								SelectGameActivity.this.startActivity(oIntent);
//							}});
//						builder.setNegativeButton("No", new DialogInterface.OnClickListener(){
//							@Override
//							public void onClick(DialogInterface arg0, int arg1) {
//								Intent oIntent = new Intent(SelectGameActivity.this, MainActivity.class);
//								oIntent.putExtra("file", strFile);
//								oIntent.putExtra("autosave", strAutoSaveFile);
//								oIntent.putExtra("resetGame", true);
//								MainActivity.loadAutoSave = false;
//								SelectGameActivity.this.startActivity(oIntent);
//							}});
//						builder.create().show();
//					}
//					else
					{
						Intent oIntent = new Intent(SelectGameActivity.this, org.libsdl.app.SDLActivity.class);
						SelectGameActivity.this.startActivity(oIntent);
					}
				}
				catch(Exception ex)
				{
					
				}
			}});
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		return true;
	}
	
	public static String getGbaFileInZip(String strZipFile)
	{
		File f = new File(strZipFile);
		return getGbaFileInZip(f);
	}
	
	public static String getGbaFileInZip(File f)
	{
		try 
		{
			java.util.zip.ZipFile zip;
			zip = new java.util.zip.ZipFile(f);
			Enumeration<? extends ZipEntry> e = zip.entries();
			while(e.hasMoreElements())
			{
				String strName = e.nextElement().getName();
				String strExtension = getExtension(strName);
				if(strExtension.compareTo("gba") == 0 || strExtension.compareTo("gb") == 0)
				{
					zip.close();
					return strName;
				}
			}
			zip.close();
		} 
		catch (ZipException e1)
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		catch (IOException e1)
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		return "";
	}
	
	public static String getExtension(String strFileName)
	{
		int n = strFileName.lastIndexOf('.');
		if(n >= 0)
		{
			return strFileName.substring(n + 1).toLowerCase();
		}
		return "";
	}
}

class MyListAdapter extends ArrayAdapter<String>
{
	Context context;
	int id;
	List<String> items;
	
	public MyListAdapter(Context context, int resource, List<String> objects) {
		super(context, resource, objects);
		this.context = context;
		id = resource;
		items = objects;
	}
	
	@Override
	public View getView(int position, View viewConvert, ViewGroup parent)
	{
		View v = viewConvert;
		if(v == null)
		{
			LayoutInflater inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			v = inflater.inflate(id, null);
			TextView tv1 = (TextView)v.findViewById(R.id.textView1);
			TextView tv2 = (TextView)v.findViewById(R.id.textView2);
			String strPath = items.get(position);
			int n = strPath.lastIndexOf('/');
			String strFileName = strPath;
			if(n >= 0)
			{
				strFileName = strPath.substring(n + 1);
			}
			tv1.setText(strFileName);
			tv2.setText(strPath);
		}
		return v;
	}
}

class ScanSDCardTask extends AsyncTask<Void, Void, Void>
{
	Context context;
	List<String> mFoundFiles = new ArrayList<String>();
	ListView mLV;
	Button mScanButton;
	
	public ScanSDCardTask(Context context, ListView lv, Button ScanButton)
	{
		mLV = lv;
		this.context = context;
		mScanButton = ScanButton;
	}
	
	@Override
	protected void onPreExecute()
	{
		mScanButton.setText("Scanning...");
	}
	
	@Override
	protected Void doInBackground(Void... arg0)
	{
		try
		{
			mFoundFiles.clear();
			String strSDCard = Environment.getExternalStorageDirectory().getAbsolutePath();
			File dir = new File(strSDCard);
			List<File> listDir = new ArrayList<File>();
			listDir.add(dir);
			while(listDir.size() > 0)
			{
				try
				{
					dir = listDir.get(0);
					listDir.remove(0);
					File[] listFile = dir.listFiles();
					if(listFile != null)
					{
						for(int i=0; i<listFile.length; i++)
						{
							if(listFile[i].isDirectory())
							{
								if(!(listFile[i].getAbsolutePath().endsWith("Android/data") ||
										listFile[i].getAbsolutePath().endsWith("Android/data/") ||
										listFile[i].getAbsolutePath().endsWith("Android/obb") ||
										listFile[i].getAbsolutePath().endsWith("Android/obb/")))
									listDir.add(listFile[i]);
							}
							else
							{
								int n = listFile[i].getName().lastIndexOf('.');
								if(n >= 0)
								{
									String strExtension = listFile[i].getName().substring(n + 1).toLowerCase();
									if(strExtension.compareTo("gba") == 0 || strExtension.compareTo("gb") == 0)
									{
										mFoundFiles.add(listFile[i].getAbsolutePath());
									}
									else if(strExtension.compareTo("zip") == 0)
									{
										try
										{
											boolean bIsNesFile = false;
											java.util.zip.ZipFile zip = new java.util.zip.ZipFile(listFile[i]);
											Enumeration<? extends ZipEntry> e = zip.entries();
											while(e.hasMoreElements())
											{
												String strName = e.nextElement().getName();
												n = strName.lastIndexOf('.');
												if(n >= 0)
												{
													strExtension = strName.substring(n + 1).toLowerCase();
													if(strExtension.compareTo("gba") == 0 || strExtension.compareTo("gb") == 0)
													{
														bIsNesFile = true;
														break;
													}
												}
											}
											
											if(bIsNesFile)
											{
												mFoundFiles.add(listFile[i].getAbsolutePath());
											}
											
										} catch (ZipException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										} catch (IOException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
										
									}
								}
							}
						}
					}
				}
				catch (Exception ex)
				{
					
				}
			}
		}
		catch(Exception ex)
		{
			
		}
		
		return null;
	}
	
	@Override
	protected void onPostExecute(Void arg)
	{
		try
		{
			mScanButton.setText("Scan SDCard");
			MyListAdapter adapter = new MyListAdapter(context, R.layout.file_view, mFoundFiles);
			mLV.setAdapter(adapter);
		}
		catch (Exception ex)
		{
			
		}
	}
}