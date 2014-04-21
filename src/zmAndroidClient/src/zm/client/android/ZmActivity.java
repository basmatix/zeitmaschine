package zm.client.android;

import java.util.ArrayList;
import java.util.List;

import android.app.ListActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnKeyListener;
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
		
		//m_adapter = new ArrayAdapter<String>( this, R.layout.flow_list_item, m_returnValues );
		m_adapter = new ZmItemArrayAdapter( this, R.layout.flow_list_item, m_returnValues );
		
		setListAdapter(m_adapter);
		
		
		m_edittext.setOnKeyListener( this );

		m_edittext.addTextChangedListener( this );
		
	    try {
	    	Log.w("zm", "create model");
	        System.out.println("test");
	    	
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
	
	public void updateStatus()
	{	
		boolean l_remoteChanges = m_gtdModel.base().hasRemoteChanges();
		boolean l_localChanges = m_gtdModel.base().hasLocalChanges();
//		m_btPull.setBackgroundColor(Color.RED);
//		m_btPush.setBackgroundColor(Color.RED);
		
		StringVec l_diffLocal = m_gtdModel.base().diff();
		StringVec l_diffRemote = m_gtdModel.base().diffRemote();
    	
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

	    m_btPull.setEnabled(l_remoteChanges);
	    m_btPush.setEnabled(l_localChanges && !l_remoteChanges );
	    
	    m_tvItemCount.setText(String.valueOf(m_gtdModel.base().getItemCount()));
	}
	
	public void onCreateContextMenu(ContextMenu menu, View v, ContextMenuInfo menuInfo) 
	{
		if (v.getId()==android.R.id.list) 
		{
		    //AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo)menuInfo;
		    menu.setHeaderTitle("title");
		    menu.add(Menu.NONE, 0, 0, "eins");
		    menu.add(Menu.NONE, 1, 1, "zwei");
		}
	}
	
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
	
	private void setListContent(String s) 
	{
		if(m_gtdModel == null)
		{
			return;
		}    	
		
		Log.i("zm", "setListContent");
		
		StringVec a;
		
		if(s.trim().equals(""))
		{
	    	a = m_gtdModel.getInboxItems(false);
		}
		else
		{
	    	a = m_gtdModel.find(s);
		}

    	m_returnValues = new ArrayList<ZmItem>();

    	for(int i = 0; i < a.size(); ++i)
    	{	
    		String l_uid = a.get(i);
    		m_returnValues.add(new ZmItem(l_uid, m_gtdModel.base().getCaption(l_uid)));
    	}

		// frans: todo: muss vmtl nicht sein..
    	m_adapter = new ZmItemArrayAdapter( this, R.layout.flow_list_item, m_returnValues);
		
		setListAdapter(m_adapter);
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		ZmItem l_item = (ZmItem)getListAdapter().getItem(position);
		Log.i("zm", "clicked on '" + l_item.m_caption + "'");
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
