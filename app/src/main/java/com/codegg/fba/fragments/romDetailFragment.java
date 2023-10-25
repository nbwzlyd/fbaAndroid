package com.codegg.fba.fragments;

import org.libsdl.app.SDLActivity;

import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.TextView;

import com.codegg.fba.ArcadeUtility;
import com.codegg.fba.R;
import com.codegg.fba.objects.RomInfo;
import com.codegg.fba.preferences.EmuPreferences;
import com.greatlittleapps.utility.FileInfo;
import com.greatlittleapps.utility.Utility;
import com.greatlittleapps.utility.UtilityMessage;

public class romDetailFragment extends Fragment
{
    public static final String ARG_FILE_PATH = "file_path";

    private AppCompatActivity activity;
    FileInfo file;
    RomInfo rom;
    
    View romDetails;
    ImageView romTitleImage;
    ImageView romScreenshotImage;
    TextView romDescriptionText;
    UtilityMessage message;
    int romDescriptionHeight = -1;
    Menu menu;
    EmuPreferences prefs;
    boolean compatListIsShown;

    public romDetailFragment() {}

    @Override
    public void onCreate(Bundle savedInstanceState) 
    {
    	 activity = (AppCompatActivity)getActivity();
         message = new UtilityMessage( this.getActivity() );
         compatListIsShown = romListFragment.compatListIsShown;
         prefs = new EmuPreferences( activity );  
         
        super.onCreate(savedInstanceState);
        setHasOptionsMenu( true );

        if (getArguments().containsKey(ARG_FILE_PATH)) 
        {
        	file = (FileInfo)Utility.unserialize( getArguments().getString( ARG_FILE_PATH ) );
        	rom = (RomInfo) file.getCustomData();
        }
    }
    
    @Override
    public void onConfigurationChanged( Configuration newConfig ) 
    {
    	super.onConfigurationChanged( newConfig );
    	
    }
    
    @Override
    public void onCreateOptionsMenu ( Menu _menu, MenuInflater inflater )
    {
    	Utility.log( "onCreateOptionsMenu" );
    	
    	super.onCreateOptionsMenu( _menu, inflater );
        inflater.inflate( R.menu.rom_detail, _menu );
        
        menu = _menu;
        
        if( menu.findItem( R.id.menu_compatList ) != null )
        	menu.findItem( R.id.menu_compatList ).setTitle( ( compatListIsShown ? "romList" : "compatList") );
        
        //if( menu.findItem( R.id.menu_back ) != null )
        //	menu.findItem( R.id.menu_back ).setVisible( !compatListIsShown );
        
        if( menu.findItem( R.id.menu_play ) != null )
        	menu.findItem( R.id.menu_play ).setVisible( !compatListIsShown );
        
        if( menu.findItem( R.id.menu_browser ) != null )
        	menu.findItem( R.id.menu_browser ).setVisible( compatListIsShown );
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) 
    {
    	Utility.log( "lxm"+item.getTitle().toString() );
    	
        //switch ( item.getItemId() )
        {
        	int id = item.getItemId();
        	if( id == R.id.menu_browser )
        	{
        		this.startActivity( Intent.createChooser( new Intent(Intent.ACTION_VIEW, Uri.parse( rom.GetGoogleLink() ) ), "Choose a browser") );
        		return true;
        	}
        	else if ( id == R.id.menu_play )
	        {
	        	prefs.setRomsPath( file.getParent() );
	        	
	        	Bundle bundle = new Bundle();
	        	bundle.putInt( "screenW", rom.GetScreenResolution().GetWidth() );
	        	bundle.putInt( "screenH", rom.GetScreenResolution().GetHeight() );
	        	bundle.putString( "data", EmuPreferences.DATA_PATH );
	        	bundle.putString( "states", EmuPreferences.STATE_PATH );
	        	bundle.putString( "roms", file.getParent() );
	        	bundle.putString( "rom", rom.GetName() );
	        	bundle.putInt( "buttons", rom.GetButtonCount() );
	        	bundle.putBoolean( "vertical", rom.GetScreenResolution().isVertical() );
	        	
	        	final Intent itent = new Intent( activity, SDLActivity.class );                
	        	//itent.setComponent( new ComponentName( UtilityPackage.AFBA_PACKAGE, UtilityPackage.AFBA_ACTIVITY ) );
	        	itent.putExtras(bundle);
	        	startActivity( itent );
	        	return true;
	        }
            //default:
        	return super.onOptionsItemSelected(item);
        }
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,Bundle savedInstanceState) 
    {
        final View rootView = inflater.inflate(R.layout.fragment_rom_detail, container, false);
        if( file != null && rom != null ) 
        {
            ((TextView) rootView.findViewById(R.id.title)).setText( rom.GetTitle() );
            
            romDetails = rootView.findViewById( R.id.romDetails );
            
            romTitleImage = (ImageView) rootView.findViewById( R.id.rom_title_image );
            romScreenshotImage = (ImageView) rootView.findViewById( R.id.rom_screenshot_image );           
            this.getScreenshots();
            
            ((TextView) rootView.findViewById(R.id.romYearView)).setText( String.valueOf( rom.GetYear() ) + " @ " + rom.GetManufacturer() );
            ((TextView) rootView.findViewById(R.id.romSystemView)).setText( "System: " + rom.GetSystem() );
            ((TextView) rootView.findViewById(R.id.romFilenameView)).setText( "File: " + rom.GetFilename() );
            
            if( rom.GetParent() != null )
            	((TextView) rootView.findViewById(R.id.romParentView)).setText( "Parent: " + rom.GetParent() );
            else
            	((TextView) rootView.findViewById(R.id.romParentView)).setVisibility( View.GONE );
            
            romDescriptionText = ((TextView) rootView.findViewById(R.id.romDescriptionView)); 
            romDescriptionText.setOnClickListener( new View.OnClickListener() 
            {
                public void onClick(View v) 
                {
                	romTitleImage.setVisibility( romTitleImage.isShown() ? View.GONE : View.VISIBLE );
                	romScreenshotImage.setVisibility( romScreenshotImage.isShown() ? View.GONE : View.VISIBLE );
                	romDetails.setVisibility( romDetails.isShown() ? View.GONE : View.VISIBLE );
                	
                	View view = rootView.findViewById( R.id.romDescription );
                	LayoutParams l = view.getLayoutParams();
                	if( romDescriptionHeight < 0 )
                		romDescriptionHeight = l.height;
                	l.height = romDetails.isShown() ? romDescriptionHeight : LayoutParams.MATCH_PARENT;
                	view.setLayoutParams( l );
                }
            });
            Thread updateRomDescriptionHandler = new Thread( new Runnable() 
            {
				@Override
				public void run() 
				{
					final String text = ArcadeUtility.getRomDescriptionOnline( rom.GetName() );
					activity.runOnUiThread( new Runnable()
					{
			            public void run()
			            {
			            	romDescriptionText.setText( text );
			            }
					});
				}  	
            });
            updateRomDescriptionHandler.start();
        }
        return rootView;
    }
    
    void getScreenshots()
    {
    	message.show( "Please wait while extracting screenshot's ..." );
    	
    	Thread th = new Thread( new Runnable()
    	{
			@Override
			public void run() 
			{
				final Bitmap[] bitmaps = ArcadeUtility.getScreenshots( file );
				activity.runOnUiThread( new Runnable()
				{
					@Override
					public void run() 
					{
						if( bitmaps[0] != null )
			            	romTitleImage.setImageBitmap( bitmaps[0] );
			            if( bitmaps[1] != null )
			            	romScreenshotImage.setImageBitmap( bitmaps[1] );
			            
			            message.hide();
					}
				});
			}	
    	});
    	th.start();
    }
}

