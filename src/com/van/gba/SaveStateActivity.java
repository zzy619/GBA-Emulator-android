package com.van.gba;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

//import com.google.ads.AdRequest;
//import com.google.ads.AdSize;
//import com.google.ads.AdView;
//import com.van.fceux2.MainActivity;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
//import com.van.fceux2.R;


public class SaveStateActivity extends Activity {
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		Log.v("gsdg", "save activity");
		
		this.setContentView(R.layout.save_state);
		
//		AdView adView = new AdView(this, AdSize.BANNER, MainActivity.admodId);
//		ViewGroup adsContainer = (ViewGroup)this.findViewById(R.id.adsContainer);
//		adsContainer.addView(adView);
//		AdRequest oAdRequest = new AdRequest();
//		adView.loadAd(oAdRequest);
		
		List<Bitmap> bms = new ArrayList<Bitmap>();
		List<String> ss = new ArrayList<String>();
		for(int i=0; i<10; i++)
			ss.add("" + i);
		
		String strPath = Environment.getExternalStorageDirectory().getAbsolutePath();
		strPath += "/Android/data/" + this.getApplication().getPackageName() + "/";
		String strTemp = org.libsdl.app.SDLActivity.mRomFile.substring(org.libsdl.app.SDLActivity.mRomFile.lastIndexOf("/") + 1);
		strTemp = strTemp.substring(0, strTemp.lastIndexOf("."));
		
		
		for(int i=0; i<10; i++)
		{
			String strStateFile = strPath + strTemp + i + ".sgm";
			//String strStateFile = strPrefix + ".fc" + i;
			File file = new File(strStateFile);
			if(file.exists())
			{
				ss.set(i, strStateFile);
				String strImageFile = strStateFile + ".png";
				File imageFile = new File(strImageFile);
				if(imageFile.exists())
				{
					Bitmap bm = BitmapFactory.decodeFile(strImageFile);
					bms.add(bm);
				}
				else
				{
					bms.add(null);
				}
			}
			else
			{
				bms.add(null);
			}
		}
		
		ListView lv = (ListView)this.findViewById(R.id.ListStates);
		
		lv.setAdapter(new SaveStateAdapter(this, R.layout.state_view, ss, bms));
		lv.setOnItemClickListener(new OnItemClickListener(){
			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2, long arg3) {
				org.libsdl.app.SDLActivity.strStateSlot = "" + arg2;// arg0.getItemAtPosition(arg2).toString();
				SaveStateActivity.this.finish();
			}});
	}
	
	@Override
	public void onBackPressed()
	{
		org.libsdl.app.SDLActivity.strStateSlot = "";
		SaveStateActivity.this.finish();
	}
}

class SaveStateAdapter extends ArrayAdapter<String>
{
	List<String> items = new ArrayList<String>();
	Context context;
	int id;
	List<Bitmap> bm;
	
	public SaveStateAdapter(Context context, int resource, List<String> objects, List<Bitmap> bm) {
		super(context, resource, objects);
		this.context = context;
		id = resource;
		items = objects;
		this.bm = bm;
	}
	
	@Override
	public View getView(int position, View viewConvert, ViewGroup parent)
	{
		View v = viewConvert;
		if(v == null)
		{
			LayoutInflater inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			v = inflater.inflate(id, null);
		}
		
		TextView tv1 = (TextView)v.findViewById(R.id.textView1);
		tv1.setText("" + position);
		TextView tv2 = (TextView)v.findViewById(R.id.textView2);
		tv2.setText(items.get(position));
		
		ImageView iv = (ImageView)v.findViewById(R.id.imageView1);
		Bitmap b = bm.get(position);
		if(b != null)
			iv.setImageBitmap(bm.get(position));
		else
			iv.setImageResource(R.drawable.icon);
		
		return v;
	}
}