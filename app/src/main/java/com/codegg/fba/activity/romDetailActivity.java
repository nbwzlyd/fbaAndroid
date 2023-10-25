package com.codegg.fba.activity;

import android.os.Bundle;
import android.view.MenuItem;

import androidx.appcompat.app.AppCompatActivity;

import com.codegg.fba.R;
import com.codegg.fba.fragments.romDetailFragment;

public class romDetailActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rom_detail);
      //  this.getSupportActionBar().setDisplayHomeAsUpEnabled( true );
        
        if (savedInstanceState == null) 
        {
            Bundle arguments = new Bundle();
            arguments.putString(romDetailFragment.ARG_FILE_PATH,
            		getIntent().getStringExtra(romDetailFragment.ARG_FILE_PATH));
            romDetailFragment fragment = new romDetailFragment();
            fragment.setArguments(arguments);
            getSupportFragmentManager().beginTransaction()
                    .add(R.id.rom_detail_container, fragment)
                    .commit();
        }
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) 
    {
        if ( item.getItemId() == android.R.id.home ) 
        {
            //NavUtils.navigateUpTo(this, new Intent(this, romListActivity.class));
        	this.finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
    
}
