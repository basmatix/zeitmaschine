package zm.client.android;

import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class ZmItemArrayAdapter extends ArrayAdapter<ZmItem> {

    Context m_context;
    int 	m_layoutResourceId;
    List<ZmItem> m_data = null;

    public ZmItemArrayAdapter(Context mContext, int layoutResourceId, List<ZmItem> data) {

        super(mContext, layoutResourceId, data);

        this.m_layoutResourceId = layoutResourceId;
        this.m_context = mContext;
        this.m_data = data;
    }

	@Override
    public View getView(int position, View convertView, ViewGroup parent) {

        /*
         * The convertView argument is essentially a "ScrapView" as described is Lucas post 
         * http://lucasr.org/2012/04/05/performance-tips-for-androids-listview/
         * It will have a non-null value when ListView is asking you recycle the row layout. 
         * So, when convertView is not null, you should simply update its contents instead of inflating a new row layout.
         */
        if(convertView==null){
            // inflate the layout
            LayoutInflater inflater = ((Activity) m_context).getLayoutInflater();
            convertView = inflater.inflate(m_layoutResourceId, parent, false);
        }

        // object item based on the position
        ZmItem objectItem = m_data.get(position);

        // get the TextView and then set the text (item name) and tag (item ID) values
        TextView textViewItem = (TextView) convertView.findViewById(R.id.textViewItem);
        textViewItem.setText(objectItem.m_caption);
        switch(objectItem.m_type)
        {
        case 0:
        	textViewItem.setTextColor(Color.RED);
        	break;
        case 1:
        	textViewItem.setTextColor(Color.BLUE);
        	break;
        case 2:
        	textViewItem.setTextColor(Color.GREEN);
        	break;
        case 3:
        	textViewItem.setTextColor(Color.GRAY);
        	break;
        }
        //textViewItem.setTag(objectItem.itemId);

        return convertView;

    }

}