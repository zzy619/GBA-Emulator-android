package com.van.gba;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.util.ArrayList;
import java.util.List;

import android.R.integer;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import org.libsdl.app.SDLActivity;


public class GamesharkActivity extends Activity {
	
	public static int selectedCheat = 0;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.gameshark);

		ListView lv = (ListView)this.findViewById(R.id.listView1);
		lv.setAdapter(new CheatAdapter(this, R.layout.cheat_view, SDLActivity.mCheats));
		
		Button oAddButton = (Button)this.findViewById(R.id.button1);
		oAddButton.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View arg0) {
				EditText et1 = (EditText) GamesharkActivity.this.findViewById(R.id.editText1);
				EditText et2 = (EditText) GamesharkActivity.this.findViewById(R.id.editText2);
				if(et2.getText() != null)
				{
					String strDescription = "";
					if(et1.getText() != null)
						strDescription = et1.getText().toString();
					
					String strCodes = et2.getText().toString().trim();
					while(strCodes.length() > 0)
					{
						String strCode = "";
						int n = strCodes.indexOf('\n');
						if(n >= 0)
						{
							strCode = strCodes.substring(0, strCodes.indexOf('\n')).trim();
							strCodes = strCodes.substring(strCodes.indexOf('\n') + 1);
						}
						else
						{
							strCode = strCodes;
							strCodes = "";
						}
						
						if(strCode.length() > 0)
						{
							strCode = strCode.toUpperCase().trim();
							if(strCode.length() == 16)
								SDLActivity.addCheat(strDescription, strCode);
							else if(strCode.length() == 12)
								SDLActivity.addCheat(strDescription, strCode.substring(0, 8) + " " + strCode.substring(8));
							else if(strCode.length() == 13 && strCode.charAt(8) == ' ')
								SDLActivity.addCheat(strDescription, strCode);
						}
					}
				}
				et1.setText("");
				et2.setText("");
				
				SDLActivity.refreshCheats();
				ListView lv = (ListView)GamesharkActivity.this.findViewById(R.id.listView1);
				lv.setAdapter(new CheatAdapter(GamesharkActivity.this, R.layout.cheat_view, SDLActivity.mCheats));
			}
		});

	}
}

class CheatAdapter extends ArrayAdapter<Cheat>
{
	List<Cheat> items = new ArrayList<Cheat>();
	Context context;
	int id;
	List<Bitmap> bm;
	View selectedViewItem = null;
	
	public CheatAdapter(Context context, int resource, List<Cheat> objects) {
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
		}
		v.setTag(Integer.toString(position));
		
		TextView tv1 = (TextView)v.findViewById(R.id.textView1);
		String enable = items.get(position).mEnable ? " (E)" : " (D)";
		tv1.setText(items.get(position).mDescription  + enable);
		TextView tv2 = (TextView)v.findViewById(R.id.textView2);
		tv2.setText(items.get(position).mCode);
		
		if(position == GamesharkActivity.selectedCheat)
		{
			v.setBackgroundColor(0xffcccccc);
			selectedViewItem = v;
		}
		else
		{
			v.setBackgroundColor(0xffffffff);
		}
		
		v.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View arg0) {
				int position = Integer.parseInt((String)arg0.getTag());
				if(position == GamesharkActivity.selectedCheat)
					return;
				if(selectedViewItem != null)
					selectedViewItem.setBackgroundColor(0xffffffff);
				selectedViewItem = arg0;
				selectedViewItem.setBackgroundColor(0xffcccccc);
				GamesharkActivity.selectedCheat = position;
			}});
		
		Button oButton = (Button) v.findViewById(R.id.button1);
		oButton.setTag(Integer.toString(position));
		oButton.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				Button b = (Button)v;
				int pos = Integer.parseInt((String)b.getTag());
				SDLActivity.deleteCheat(pos);
				SDLActivity.refreshCheats();
				CheatAdapter.this.notifyDataSetChanged();
			}});
		
		return v;
	}
}