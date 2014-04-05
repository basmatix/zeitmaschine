package zm.client.android;

//import android.app.Activity;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.ListActivity;
import android.os.Bundle;
import android.os.Debug;
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

	List<String> m_returnValues = new ArrayList<String>();
	
	ArrayAdapter<String> m_adapter;
	
	zmGtdModel m_gtdModel;
	
	Button m_btSync;
	Button m_btAdd;
	EditText m_edittext;
	
	/// Called when the activity is first created.
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		
		final ListView l_list = (ListView)findViewById(android.R.id.list);
		m_edittext = (EditText)findViewById(R.id.editText1);
		
		registerForContextMenu( l_list );
		
		m_adapter = new ArrayAdapter<String>( this, R.layout.flow_list_item, m_returnValues );
		
		setListAdapter(m_adapter);
		
		
		m_edittext.setOnKeyListener( this );

		m_edittext.addTextChangedListener( this );
		
		//helloLog("onCreate");

		//zmStartup( this.getApplicationContext().getFilesDir().toString(),
		//			 Environment.getExternalStorageDirectory().toString());

	
	
	    try {
	    	Log.w("zm", "create model");
	        System.out.println("test");
	    	
			//theModel = MindMatterModel.create_new();
	        m_gtdModel = new zmGtdModel(MindMatterModel.create());
	        //m_baseModel = MindMatterModel.create();
	        m_gtdModel.base().setTraceLevel(4);
	        m_gtdModel.initialize();

	    	Log.i("zm", "found " + m_gtdModel.base().getItemCount() + " items");

			/*
			theModel.createNewItem("Get milk!");
			//MindMatterModel theModel = new MindMatterModel();
						
			theModel.createNewItem("test");
			*/
	    } catch (Exception e) {
	        // gib die Fehlermeldung aus
	        System.out.println("ERROR: "+e.getMessage());
	    }		
		
	    m_btSync = (Button) findViewById(R.id.btSync);
	    m_btAdd = (Button) findViewById(R.id.btAdd);
	    
		m_btSync.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
 
		    	Log.w("zm", "sync");
		    	m_gtdModel.base().sync();
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
			}
		});		
		
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
		//zmSetMetaString( s );
		
		//m_returnValues = Arrays.asList( zmGetMatchingItems() );
		StringVec a;
		
		if(s.trim().equals(""))
		{
	    	a = m_gtdModel.getInboxItems(false);
		}
		else
		{
	    	a = m_gtdModel.find(s);
		}

    	m_returnValues = new ArrayList<String>();

    	for(int i = 0; i < a.size(); ++i)
    	{	
    		String es = a.get(i);
    		m_returnValues.add(m_gtdModel.base().getCaption(es));
    	}
    	

		// frans: todo: muss vmtl nicht sein..
    	m_adapter = new ArrayAdapter<String>( this, R.layout.flow_list_item, m_returnValues);
		
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
