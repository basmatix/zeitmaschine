package zm.client.android;

import java.util.ArrayList;
import java.util.List;

import android.app.AlertDialog;
import android.app.ListActivity;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.widget.AdapterView.AdapterContextMenuInfo;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnKeyListener;
import android.util.DisplayMetrics;
import android.util.Log;
import android.widget.Button;
import android.view.View.OnClickListener;
import zm.client.jni.*;
/// sources:
/// http://www.mikeplate.com/2010/01/21/show-a-context-menu-for-long-clicks-in-an-android-listview/


public class ZmActivity 
	extends ListActivity 
	implements TextWatcher, OnKeyListener
{
	static { System.loadLibrary("javaZm"); }

	List<ZmItem> m_returnValues = new ArrayList<ZmItem>();
	
	ZmItemArrayAdapter m_adapter;
	
	zmGtdModel m_gtdModel;
	
	Button m_btPull;
	Button m_btPush;
	Button m_btAdd;
	Button m_btChoose;
	EditText m_edittext;
	ListView m_listView;
	TextView m_tvItemCount;
	
	/// Called when the activity is first created.
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		
		m_listView = (ListView)findViewById(android.R.id.list);
		m_edittext = (EditText)findViewById(R.id.tbMultiText);
		
		registerForContextMenu( m_listView );
		
		m_adapter = new ZmItemArrayAdapter( this, R.layout.flow_list_item, m_returnValues );
		
		setListAdapter(m_adapter);
		
		m_edittext.setOnKeyListener( this );

		m_edittext.addTextChangedListener( this );
		
	    try {
	    	Log.w("zm", "create model");
	    	
			//theModel = MindMatterModel.create_new();
	        m_gtdModel = new zmGtdModel(MindMatterModel.create());
	        //m_baseModel = MindMatterModel.create();
	        m_gtdModel.base().setTraceLevel(4);
	        m_gtdModel.initialize();

	    	Log.i("zm", "found " + m_gtdModel.base().getItemCount() + " items");

	    } catch (Exception e) {
	        // gib die Fehlermeldung aus
	        System.out.println("ERROR: "+e.getMessage());
	    }		
		
	    m_btPull = (Button) findViewById(R.id.btPull);
	    m_btPush = (Button) findViewById(R.id.btPush);
	    m_btAdd = (Button) findViewById(R.id.btAdd);
	    m_btChoose = (Button) findViewById(R.id.btChoose);
	    m_tvItemCount = (TextView) findViewById(R.id.tvItemCount);

	    updateStatus();
	    
		m_btPull.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
 
		    	Log.w("zm", "sync-pull");
		    	boolean l_result = m_gtdModel.base().sync_pull();
		    	Log.w("zm", "returned " + l_result);
		    	updateStatus();
		    	m_gtdModel.base().saveLocal();
			}
		});
		m_btPush.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
 
		    	Log.w("zm", "sync-push");
		    	boolean l_result = m_gtdModel.base().sync_push();
		    	Log.w("zm", "returned " + l_result);
		    	updateStatus();
		    	m_gtdModel.base().saveLocal();
			}
		});

		m_btChoose.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				ZmItem l_todo = m_returnValues.get((int)(Math.random() * m_returnValues.size()));
				new AlertDialog.Builder(ZmActivity.this)
				       .setTitle("jetzt:")
				       .setMessage(l_todo.m_caption)
					   .create()
		    	       .show();
			}
		});
		
		m_btAdd.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				if(m_gtdModel == null)
				{
					return;
				}    			
				String l_text = m_edittext.getText().toString();
				if(l_text.trim().equals(""))
				{
					return;
				}
		    	Log.w("zm", "add " + l_text);
		    	m_gtdModel.createNewInboxItem(l_text);
		    	m_gtdModel.base().saveLocal();
		    	m_edittext.setText("");
		    	updateStatus();
			}
		});
	
		setListContent( "" );
	}

	public boolean onCreateOptionsMenu(Menu menu) {
        
	    MenuInflater inflater = getMenuInflater();
	    inflater.inflate(R.menu.main_menu, menu);
	    return true;
	}
	
	public boolean onOptionsItemSelected(MenuItem item) 
	{
		switch (item.getItemId()){		     
	    case R.id.sync_status:
	    {
			StringVec l_diffLocal = m_gtdModel.base().diffLocal();
			StringVec l_diffRemote = m_gtdModel.base().diffRemote();
			
			String l_statusString = "";
			
			l_statusString += "local (changes you would push):\n  ";
	    	
	    	for(int i = 0; i < l_diffLocal.size(); ++i)
	    	{
				l_statusString += l_diffLocal.get(i) + "\n  ";
	    	}

	    	l_statusString += "\nremote (changes you would pull):\n  ";
	    	
	    	for(int i = 0; i < l_diffRemote.size(); ++i)
	    	{			
				l_statusString += l_diffRemote.get(i) + "\n  ";
	    	}
	    	
    	
			// 1. Instantiate an AlertDialog.Builder with its constructor
			AlertDialog.Builder builder = new AlertDialog.Builder(ZmActivity.this);
			
			// 2. Chain together various setter methods to set the dialog characteristics
			builder.setTitle("to sync or not to sync?")
			       .setMessage(l_statusString);
			// 3. Get the AlertDialog from create()
			AlertDialog dialog = builder.create();
			DisplayMetrics metrics = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(metrics);
			
	    	dialog.show();	    
			
	    	TextView textView = (TextView) dialog.findViewById(android.R.id.message);
	    	textView.setTypeface(Typeface.MONOSPACE);
	    	textView.setTextSize(8);
	    	dialog.getWindow().setLayout(metrics.widthPixels, metrics.heightPixels);
	    	
			return true;
	    }
	    case R.id.about:
	    return true;
	    default:
	    return super.onOptionsItemSelected(item);	
		}
	}
	
	public void updateStatus()
	{	
		boolean l_remoteChanges = m_gtdModel.base().hasRemoteChanges();
		boolean l_localChanges = m_gtdModel.base().hasLocalChanges();
//		m_btPull.setBackgroundColor(Color.RED);
//		m_btPush.setBackgroundColor(Color.RED);
		
		StringVec l_diffLocal = m_gtdModel.base().diffLocal();
		StringVec l_diffRemote = m_gtdModel.base().diffRemote();
    	
	    try
	    {
			Log.w("zm", "diffs local: " + l_localChanges + " " + l_diffLocal.size());
	    	for(int i = 0; i < l_diffLocal.size(); ++i)
	    	{
				Log.w("zm", "  -" + l_diffLocal.get(i));
	    	}
	    	
	    	Log.w("zm", "diffs remote: " + l_remoteChanges + " " + l_diffRemote.size());
	    	for(int i = 0; i < l_diffRemote.size(); ++i)
	    	{			
		    	Log.w("zm", "  -" + l_diffRemote.get(i));
	    	}
	    } catch (Exception e) {
	        System.out.println("ERROR: "+e.getMessage());
	    }		

	    m_btPull.setEnabled(l_remoteChanges);
	    m_btPush.setEnabled(l_localChanges && !l_remoteChanges );
	    
	    m_tvItemCount.setText(String.valueOf(m_gtdModel.base().getItemCount()));
	}

	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) 
	{
		super.onCreateContextMenu(menu, v, menuInfo);  	
		if (v.getId()==android.R.id.list) 
		{
			AdapterContextMenuInfo info = (AdapterContextMenuInfo) menuInfo;
			ZmItem l_todo = m_returnValues.get(info.position);
		        
		    //AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo)menuInfo;
		    menu.setHeaderTitle(l_todo.m_caption);
		    menu.add(Menu.NONE, info.position, 0, "set done");
		    menu.add(Menu.NONE, info.position, 1, "connect..");
		    menu.add(Menu.NONE, info.position, 2, "to: task");
		    menu.add(Menu.NONE, info.position, 3, "to: project");
		    menu.add(Menu.NONE, info.position, 4, "to: knowledge");
		    menu.add(Menu.NONE, info.position, 5, "to: kaufen");
		    menu.add(Menu.NONE, info.position, 6, "to: film");
		}
	}
	
   @Override  
   public boolean onContextItemSelected(MenuItem item)
   {
		ZmItem l_todo = m_returnValues.get(item.getItemId());
	   	String l_title = (String) item.getTitle() + " " + item.getOrder() + " " + item.getItemId() + "\n" + l_todo.m_caption;
	   	switch(item.getOrder())
	   	{
	   	case 0: // done
	   		m_gtdModel.setDone(l_todo.m_uid);
			new AlertDialog.Builder(ZmActivity.this)
		       .setTitle("set to done:")
		       .setMessage(l_title).create().show();
	   		break;
	   	case 1: // connect
	   		break;
	   	case 2: // task
	   		break;
	   	case 3: // project
	   		m_gtdModel.castToProject(l_todo.m_uid);
			new AlertDialog.Builder(ZmActivity.this)
		       .setTitle("cast to project:")
		       .setMessage(l_title).create().show();
	   		break;
	   	case 4: // knowledge
	   		break;
	   	case 5: // kaufen
	   		break;
	   	case 6: // film
	   		break;
	   	}

	   	m_gtdModel.base().saveLocal();
	   	setListContent(m_edittext.getText().toString());
		
		return true;
   }
	
	//public void on
    public boolean onKey(View v, int keyCode, KeyEvent event) 
    {
        if( (event.getAction() == KeyEvent.ACTION_DOWN) && (keyCode == KeyEvent.KEYCODE_ENTER)) 
        {
			setListContent( "" );
			return true;
        }
        return false;
    }
	
	public void onDestroy() 
	{
		super.onDestroy();
		//helloLog("onDestroy.\n");
		//zmShutdown();
	}

	/// can we avoid this?
	public void afterTextChanged(Editable s) { }

	/// can we avoid this?
	public void beforeTextChanged(CharSequence s, int start, int count,	int after) { }

	public void onTextChanged(CharSequence s, int start, int before, int count) 
	{		
    	Log.i("zm", "onTextChanged");
		setListContent( s.toString() );
	}
	
	private void addViewItems(StringVec uids, int type)
	{
    	for(int i = 0; i < uids.size(); ++i)
    	{	
    		String l_uid = uids.get(i);
    		m_returnValues.add(new ZmItem(l_uid, m_gtdModel.base().getCaption(l_uid), type));
    	}
	}
	
	private void setListContent(String s) 
	{
		if(m_gtdModel == null)
		{
			return;
		}    	
		
		Log.i("zm", "setListContent");

    	m_returnValues = new ArrayList<ZmItem>();
		
		if(s.trim().equals(""))
		{
	    	addViewItems(m_gtdModel.getInboxItems(false),0);
	    	addViewItems(m_gtdModel.getTaskItems(true,false),1);
	    	addViewItems(m_gtdModel.getProjectItems(false,false),2);
		}
		else
		{
	    	addViewItems(m_gtdModel.find(s), 3);
		}

		// frans: todo: muss vmtl nicht sein..
    	m_adapter = new ZmItemArrayAdapter(this, R.layout.flow_list_item, m_returnValues);
		
		setListAdapter(m_adapter);
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		ZmItem l_item = (ZmItem)getListAdapter().getItem(position);
		Log.i("zm", "clicked on '" + l_item.m_caption + "'");
		
		AlertDialog.Builder builder = new AlertDialog.Builder(ZmActivity.this);
		
		builder.setTitle(l_item.m_caption)
		       .setMessage(m_gtdModel.getNote(l_item.m_uid));

		AlertDialog dialog = builder.create();
		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		
    	dialog.show();	    
		
    	TextView textView = (TextView) dialog.findViewById(android.R.id.message);
    	textView.setTypeface(Typeface.MONOSPACE);
    	textView.setTextSize(8);
    	dialog.getWindow().setLayout(metrics.widthPixels, metrics.heightPixels);		
	}
/*
	public void onStart()   { super.onStart();   helloLog("onStart"); }

	public void onRestart() { super.onRestart(); helloLog("onRestart"); }

	public void onResume()  { super.onResume();  helloLog("onResume"); }

	public void onPause()   { super.onPause();   helloLog("onPause: bye bye!"); }

	public void onStop()    { super.onStop();    helloLog("onStop."); }

	public void onMessage() { helloLog("onMessage"); }
*/
}
