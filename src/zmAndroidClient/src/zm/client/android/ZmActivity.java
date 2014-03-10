package zm.client.android;

//import android.app.Activity;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.ListActivity;
import android.os.Bundle;
import android.os.Environment;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.ContextMenu;
import android.view.ContextMenu.ContextMenuInfo;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.ArrayAdapter;

/// sources:
/// http://www.mikeplate.com/2010/01/21/show-a-context-menu-for-long-clicks-in-an-android-listview/


public class ZmActivity 
	extends ListActivity 
	implements TextWatcher, OnKeyListener 
{
	//static { System.loadLibrary("zmCoreWrapper"); }

	///
	/// native methods
	///
	/*
	private native void helloLog(String logThis);
	
	/// will startup a zm instance and load the graph
	private native void zmStartup( String localDirectory, String externalDirectory );
	
	/// will save the graph and destroy the zm instance
	private native void zmShutdown();
	
	/// sends a meta string to the zm instance
	private native void zmSetMetaString( String logThis );
	
	/// gets a list of matching items in the zm graph
	private native String[] zmGetMatchingItems();
	
	/// executes a previously set meta string
	private native void zmCommitMetaString();
	
    */
	List<String> m_returnValues = new ArrayList<String>();
	ArrayAdapter<String> m_adapter;
	/// Called when the activity is first created.
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		
		final ListView l_list = (ListView)findViewById(android.R.id.list);
		final EditText l_edittext = (EditText)findViewById(R.id.editText1);
		
		registerForContextMenu( l_list );
		
		m_adapter = new ArrayAdapter<String>( this, R.layout.flow_list_item, m_returnValues );
		
		setListAdapter(m_adapter);
		
		
		l_edittext.setOnKeyListener( this );

		l_edittext.addTextChangedListener( this );
		
		//helloLog("onCreate");

		//zmStartup( this.getApplicationContext().getFilesDir().toString(),
		//			 Environment.getExternalStorageDirectory().toString());

		setListContent( "" );
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
			//zmCommitMetaString();
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
		//helloLog("onTextChanged");
		setListContent( s.toString() );
	}
	
	private void setListContent(String s) 
	{		
		//zmSetMetaString( s );
		
		//m_returnValues = Arrays.asList( zmGetMatchingItems() );
		
		// frans: todo: muss vmtl nicht sein..
		m_adapter = new ArrayAdapter<String>( this, R.layout.flow_list_item, m_returnValues );
		
		setListAdapter(m_adapter);
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		//String item = (String) getListAdapter().getItem(position);
		//helloLog("onListItemClick");
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
