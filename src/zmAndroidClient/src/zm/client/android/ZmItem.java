package zm.client.android;

public class ZmItem {

    public String m_uid;
    public String m_caption;
    public int m_type;
    // color
    //
    
    // constructor
    public ZmItem(String uid, String caption, int type) {
        this.m_uid = uid;
        this.m_caption = caption;
        this.m_type = type;
    }
}