package org.libsdl.app;

import android.widget.HeaderViewListAdapter;
import android.widget.ListAdapter;
import android.widget.ListView;

import androidx.appcompat.app.AppCompatActivity;

public abstract class ActionBarListActivity extends AppCompatActivity {

private ListView mListView;

protected ListView getListView() {
    if (mListView == null) {
        mListView = (ListView) findViewById(android.R.id.list);
    }
    return mListView;
}

protected void setListAdapter(ListAdapter adapter) {
    getListView().setAdapter(adapter);
}

protected ListAdapter getListAdapter() {
    ListAdapter adapter = getListView().getAdapter();
    if (adapter instanceof HeaderViewListAdapter) {
        return ((HeaderViewListAdapter)adapter).getWrappedAdapter();
    } else {
        return adapter;
    }
}
}
