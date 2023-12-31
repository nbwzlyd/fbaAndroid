package org.libsdl.app;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.InputType;
import android.util.Log;
import android.util.SparseArray;
import android.view.Display;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.view.ActionMode;

import com.codegg.fba.R;
import com.codegg.fba.effects.Effect;
import com.codegg.fba.effects.EffectList;
import com.codegg.fba.emulator.StateAdapter;
import com.codegg.fba.emulator.StateInfo;
import com.codegg.fba.emulator.StateList;
import com.codegg.fba.emulator.input.HardwareInput;
import com.codegg.fba.emulator.input.IButtons;
import com.codegg.fba.emulator.input.SoftwareInputView;
import com.codegg.fba.emulator.sdl.SDLSurface;
import com.codegg.fba.emulator.utility.EmuPreferences;
import com.greatlittleapps.utility.Utility;
import com.greatlittleapps.utility.UtilityMessage;



/**
 * SDL Activity
 */
public class SDLActivity extends ActionBarListActivity implements OnKeyListener {
	public static SDLActivity activity;

	private static UtilityMessage mMessage;

	public EmuPreferences mPrefs;
	public Effect effectView;
	public EffectList mEffectList;

	private RelativeLayout mainView;
	private SoftwareInputView inputView;
	private HardwareInput inputHardware;
    private static final String TAG = "aFBA";
	public static native void setfskip(int n);

	public static native int ispaused();

	public static native void pauseemu();

	public static native void resumeemu();

	public static native int getslotnum();

	public static native void statesave(int statenum);

	public static native void stateload(int statenum);

	public static native void emustop();

	public static native void setPadSwitch();

	public static native void nativeInitWithArgs(String[] pArgs);

	public static native void onNativeResize(int x, int y, int format);

	public static native void nativeRunAudioThread();

	public static native void nativePause();

	public static native void nativeResume();

	// C functions we call
	public static native int nativeInit(Object arguments);

	public static native void nativeLowMemory();

	public static native void nativeQuit();

	public static native void onNativeDropFile(String filename);

	public static native void onNativeResize(int x, int y, int format,
			float rate);

	public static native int onNativePadDown(int device_id, int keycode);

	public static native int onNativePadUp(int device_id, int keycode);

	public static native void onNativeJoy(int device_id, int axis, float value);

	public static native void onNativeHat(int device_id, int hat_id, int x,
			int y);

   public static native void setPadData(int pad, long pad1);
	
	public static native void onNativeKeyDown(int keycode);

	public static native void onNativeKeyUp(int keycode);

	public static native void onNativeKeyboardFocusLost();

	public static native void onNativeMouse(int button, int action, float x,
			float y);

	public static native void onNativeTouch(int touchDevId,
			int pointerFingerId, int action, float x, float y, float p);

	public static native void onNativeAccel(float x, float y, float z);

	public static native void onNativeSurfaceChanged();

	public static native void onNativeSurfaceDestroyed();

	public static native int nativeAddJoystick(int device_id, String name,
			int is_accelerometer, int nbuttons, int naxes, int nhats, int nballs);

	public static native int nativeRemoveJoystick(int device_id);

	public static native String nativeGetHint(String name);

	// Keep track of the paused state
	public static boolean mIsPaused, mIsSurfaceReady, mHasFocus;
	public static boolean mExitCalledFromJava;

	/**
	 * If shared libraries (e.g. SDL or the native application) could not be
	 * loaded.
	 */
	public static boolean mBrokenLibraries;

	// If we want to separate mouse and touch events.
	// This is only toggled in native code when a hint is set!
	public static boolean mSeparateMouseAndTouch;


	protected static SDLSurface mSurface;
	protected static View mTextEdit;
	protected static ViewGroup mLayout;
	protected static SDLJoystickHandler mJoystickHandler;

	// Audio
	protected static AudioTrack mAudioTrack;
	protected static AudioRecord mAudioRecord;

	// This is what SDL runs in. It invokes SDL_main(), eventually
	public static Thread mSDLThread;

	public static String rom;
	public static String datapath;
	public static String rompath;
	public static String statespath;
	public static String cachepath;

	public static String getDataPath() {
		return datapath;
	}

	public static String getRomsPath() {
		return rompath;
	}

	public static String getCachePath() {
		return cachepath;
	}

	public static int mScreenHolderSizeX = 320;
	public static int buttonCount = 4;
	public static int mScreenHolderSizeY = 240;
	public static int mScreenEmuSizeX = 320;
	public static int mScreenEmuSizeY = 240;
	public static boolean vertical = false;
	public static String[] args = null;

	/**
	 * This method is called by SDL before loading the native shared libraries.
	 * It can be overridden to provide names of shared libraries to be loaded.
	 * The default implementation returns the defaults. It never returns null.
	 * An array returned by a new implementation must at least contain "SDL2".
	 * Also keep in mind that the order the libraries are loaded may matter.
	 * 
	 * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
	 */
	// 加载库
	protected String[] getLibraries() {
		return new String[] { "SDL2", "SDL2_image", "SDL2_mixer", "SDL2_net",
				"SDL2_ttf", "mikmod", "smpeg2", "arcade" };
	}

	// Load the .so
	public void loadLibraries() {
		for (String lib : getLibraries()) {
			System.loadLibrary(lib);
		}
	}

	private AlertDialog inputHardwareDialog;
	private int inputHardwareButtonNow = 0;
	private boolean inputHardwareEdit = false;

	private ActionBar actionBar;
	private Menu menu;
	public ListView stateMenu; // "@+id/android:list"
	public StateAdapter statesAdapter;
	Display display;

	public static int actionBarHeight;

	public static void initialize() {
		// The static nature of the singleton and Android quirkyness force us to
		// initialize everything here
		// Otherwise, when exiting the app and returning to it, these variables
		// *keep* their pre exit values
		mSurface = null;
		mTextEdit = null;
		mLayout = null;
		mJoystickHandler = null;
		mSDLThread = null;
		mAudioTrack = null;
		mAudioRecord = null;
		mExitCalledFromJava = false;
		mBrokenLibraries = false;
		mIsPaused = false;
		mIsSurfaceReady = false;
		mHasFocus = true;
		activity=null;
	}
    /**
     * This method is called by SDL using JNI.
     */
    public static boolean setActivityTitle(String title) {
        // Called from SDLMain() thread and can't directly affect the view
        return activity.sendCommand(COMMAND_CHANGE_TITLE, title);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean sendMessage(int command, int param) {
        return activity.sendCommand(command, Integer.valueOf(param));
    }
	// Setup
	@SuppressLint({ "InlinedApi", "InflateParams" })
	// for Window.FEATURE_ACTION_BAR_OVERLAY
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		initialize();

		// Load shared libraries
		String errorMsgBrokenLib = "";
		try {
			loadLibraries();
		} catch (UnsatisfiedLinkError e) {
			System.err.println(e.getMessage());
			mBrokenLibraries = true;
			errorMsgBrokenLib = e.getMessage();
		} catch (Exception e) {
			System.err.println(e.getMessage());
			mBrokenLibraries = true;
			errorMsgBrokenLib = e.getMessage();
		}

		if (mBrokenLibraries) {
			AlertDialog.Builder dlgAlert = new AlertDialog.Builder(this);
			dlgAlert.setMessage("An error occurred while trying to start the application. Please try again and/or reinstall."
					+ System.getProperty("line.separator")
					+ System.getProperty("line.separator")
					+ "Error: "
					+ errorMsgBrokenLib);
			dlgAlert.setTitle("SDL Error");
			dlgAlert.setPositiveButton("Exit",
					new DialogInterface.OnClickListener() {
						@Override
						public void onClick(DialogInterface dialog, int id) {
							// if this button is clicked, close current activity
							SDLActivity.activity.finish();
						}
					});
			dlgAlert.setCancelable(false);
			dlgAlert.create().show();

			return;
		}
		if (Build.VERSION.SDK_INT >= 12) {
			mJoystickHandler = new SDLJoystickHandler_API12();
		} else {
			mJoystickHandler = new SDLJoystickHandler();
		}
		display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE))
				.getDefaultDisplay();

		// force overflow menu
		try {
			ViewConfiguration config = ViewConfiguration.get(this);
			Field menuKeyField = ViewConfiguration.class
					.getDeclaredField("sHasPermanentMenuKey");
			if (menuKeyField != null) {
				menuKeyField.setAccessible(true);
				menuKeyField.setBoolean(config, false);
			}
		} catch (Exception ex) {
		}

		if (this.getIntent().getExtras() != null) {
			vertical = this.getIntent().getExtras().getBoolean("vertical");
			buttonCount = this.getIntent().getExtras().getInt("buttons");
			mScreenHolderSizeX = this.getIntent().getExtras().getInt("screenW");
			mScreenHolderSizeY = this.getIntent().getExtras().getInt("screenH");
			mScreenEmuSizeX = mScreenHolderSizeX;
			mScreenEmuSizeY = mScreenHolderSizeY;
			datapath = this.getIntent().getExtras().getString("data");
			statespath = this.getIntent().getExtras().getString("states");
			rompath = this.getIntent().getExtras().getString("roms");
			rom = this.getIntent().getExtras().getString("rom");
		} else {
			vertical = true;
			buttonCount = 2;
			mScreenHolderSizeX = 448;
			mScreenHolderSizeY = 224;
			mScreenEmuSizeX = mScreenHolderSizeX;
			mScreenEmuSizeY = mScreenHolderSizeY;
			datapath = Environment.getExternalStorageDirectory().getPath()
					+ "/aFBA";
			statespath = Environment.getExternalStorageDirectory().getPath()
					+ "/aFBA/states";
			rompath = Environment.getExternalStorageDirectory().getPath()
					+ "/aFBA/roms";
			rom = "oldsplus";
		}

		args = new String[1];
		args[0] = rom;

		activity = this;

		mPrefs = new EmuPreferences(SDLActivity.this);
		mMessage = new UtilityMessage(SDLActivity.this);
		mEffectList = new EffectList();
		inputHardware = new HardwareInput(SDLActivity.this);

		LayoutInflater layoutInflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
		mainView = (RelativeLayout) layoutInflater.inflate(R.layout.emulator,
				null);

		inputView = new SoftwareInputView(display, getPackageName(), mainView,
				buttonCount, mPrefs.useVibration(), false);
		mainView.addView(inputView);

		effectView = new Effect(this);
		mainView.addView(effectView);

		setContentView(mainView);

		mSurface = (SDLSurface) mainView.findViewById(R.id.surface);
		mSurface.setKeepScreenOn(true);
		mSurface.setOnKeyListener(this);

		stateMenu = (ListView) this.findViewById(android.R.id.list);
		stateMenu.setOnItemClickListener(statesListener);
		stateMenu.setVisibility(View.GONE);
		statesAdapter = new StateAdapter(activity, R.layout.statelist);
		activity.setListAdapter(this.statesAdapter);

		applyRatioAndEffect();
		actionBar = this.getSupportActionBar();
		actionBar.setDisplayHomeAsUpEnabled(true);
		this.hideNavBar();

		if (!mPrefs.useSwInput())
			inputView.setVisibility(View.GONE);
		else {
			inputView.requestFocus();
			inputView.bringToFront();
		}

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			uiChangeListener();
		}
	}

	public void updateStatesList() {
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				statesAdapter.clear();
				statesAdapter.add(new StateInfo(getResources().getDrawable(
						R.drawable.state)));

				final StateList statelist = new StateList(statespath, rom);
				for (int i = 0; i < statelist.getStates().size(); i++) {
					statesAdapter.add(statelist.getStates().get(i));
				}
				stateMenu.setVisibility(View.VISIBLE);
				stateMenu.bringToFront();
				stateMenu.setFocusable(true);
				stateMenu.requestFocus();
			}
		});
	}

	private OnItemClickListener statesListener = new OnItemClickListener() {
		@Override
		public void onItemClick(AdapterView<?> parent, View v,
				final int position, final long id) {
			final StateInfo state = statesAdapter.getItem((int) id);
			if (state.date.contentEquals("Create new save")) {
				final int num = statesAdapter.getCount() - 1;
				Utility.log("Saving state in slot " + num);
				statesave(num);

				updateStatesList();
			} else {
				// dialogStates( state );
				Utility.log("Loading state from slot " + state.id);
				stateload(state.id);
				handlePauseMenu();
			}
		}
	};

	@Override
	public boolean onCreateOptionsMenu(Menu pMenu) {
		getMenuInflater().inflate(R.menu.menu, pMenu);
		menu = pMenu;
		menu.findItem(R.id.menu_input_usesw).setChecked(mPrefs.useSwInput());
		menu.findItem(R.id.menu_input_vibrate)
				.setChecked(mPrefs.useVibration());
		updateFskip(mPrefs.getFrameSkip());
		return super.onCreateOptionsMenu(menu);
	}

	@SuppressLint("NewApi")
	public void uiChangeListener() {
		final View decorView = getWindow().getDecorView();
		decorView
				.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
					@SuppressLint("NewApi")
					@Override
					public void onSystemUiVisibilityChange(int visibility) {
						if ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0) {
							decorView
									.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
											| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
											| View.SYSTEM_UI_FLAG_FULLSCREEN
											| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
						}
					}
				});
	}

	@SuppressLint("NewApi")
	public void hideNavBar() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			mainView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
					| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
					| View.SYSTEM_UI_FLAG_FULLSCREEN
					| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		}
		actionBar.hide();
	}

	public void showNavBar() {
		actionBar.show();
	}

	public void updateFskip(int fskip) {
		// handle fskip checkbox's
		menu.findItem(R.id.menu_fskip_0).setChecked(fskip == 0 ? true : false);
		menu.findItem(R.id.menu_fskip_1).setChecked(fskip == 1 ? true : false);
		menu.findItem(R.id.menu_fskip_2).setChecked(fskip == 2 ? true : false);
		menu.findItem(R.id.menu_fskip_3).setChecked(fskip == 3 ? true : false);
		menu.findItem(R.id.menu_fskip_4).setChecked(fskip == 4 ? true : false);
		menu.findItem(R.id.menu_fskip_5).setChecked(fskip == 5 ? true : false);
		menu.findItem(R.id.menu_fskip_6).setChecked(fskip == 6 ? true : false);
		menu.findItem(R.id.menu_fskip_7).setChecked(fskip == 7 ? true : false);
		menu.findItem(R.id.menu_fskip_8).setChecked(fskip == 8 ? true : false);
		menu.findItem(R.id.menu_fskip_9).setChecked(fskip == 9 ? true : false);

		setfskip(fskip);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		int pad_data = 0;

		int itemId = item.getItemId();
		if (itemId == R.id.menu_scale) {
			int newSize = mPrefs.getScreenSize();
			newSize++;
			if (newSize >= EffectList.ScreenSize.values().length) {
				newSize = 0;
			}
			mPrefs.setScreenSize(newSize);
			applyRatioAndEffect();
			return true;
		} else if (itemId == R.id.menu_effects) {
			selectEffect();
			return true;
		} else if (itemId == R.id.menu_fskip_0) {
			mPrefs.setFrameSkip(0);
			updateFskip(0);
			return true;
		} else if (itemId == R.id.menu_fskip_1) {
			mPrefs.setFrameSkip(1);
			updateFskip(1);
			return true;
		} else if (itemId == R.id.menu_fskip_2) {
			mPrefs.setFrameSkip(2);
			updateFskip(2);
			return true;
		} else if (itemId == R.id.menu_fskip_3) {
			mPrefs.setFrameSkip(3);
			updateFskip(3);
			return true;
		} else if (itemId == R.id.menu_fskip_4) {
			mPrefs.setFrameSkip(4);
			updateFskip(4);
			return true;
		} else if (itemId == R.id.menu_fskip_5) {
			mPrefs.setFrameSkip(5);
			updateFskip(5);
			return true;
		} else if (itemId == R.id.menu_fskip_6) {
			mPrefs.setFrameSkip(6);
			updateFskip(6);
			return true;
		} else if (itemId == R.id.menu_fskip_7) {
			mPrefs.setFrameSkip(7);
			updateFskip(7);
			return true;
		} else if (itemId == R.id.menu_fskip_8) {
			mPrefs.setFrameSkip(8);
			updateFskip(8);
			return true;
		} else if (itemId == R.id.menu_fskip_9) {
			mPrefs.setFrameSkip(9);
			updateFskip(9);
			return true;
		} else if (itemId == R.id.menu_states) {
			updateStatesList();
			return true;
		} else if (itemId == R.id.menu_input_edit) {
			AlertDialog alertDialog = new AlertDialog.Builder(SDLActivity.this)
					.create();
			alertDialog.setTitle("Choose");
			alertDialog
					.setMessage("You can either edit the default configuration for all games with "
							+ buttonCount
							+ " buttons or for this game ("
							+ rom
							+ ")");
			alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, "Default "
					+ buttonCount + " buttons",
					new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int which) {
							startSupportActionMode(new InputEditActionMode(
									buttonCount));
						}
					});
			alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, "This game: "
					+ rom, new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog, int which) {
					startSupportActionMode(new InputEditActionMode(-1));
				}
			});
			alertDialog.show();
			return true;
		} else if (itemId == R.id.menu_input_vibrate) {
			boolean vibrate = !mPrefs.useVibration();
			mPrefs.useVibration(vibrate);
			inputView.setVibration(vibrate);
			item.setChecked(vibrate);
			return true;
		} else if (itemId == R.id.menu_input_usesw) {
			boolean useSw = !mPrefs.useSwInput();
			mPrefs.useSwInput(useSw);
			inputView.setVisibility(useSw ? View.VISIBLE : View.GONE);
			item.setChecked(useSw);
			return true;
		} else if (itemId == R.id.menu_input_sethw) {
			showInputHardwareDialog();
			return true;
		} else if (itemId == R.id.menu_switchs_service) {
			handlePauseMenu();
			pad_data = 0;
			pad_data |= IButtons.VALUE_TEST;
			SDLActivity.setPadData(0, pad_data);
			return true;
		} else if (itemId == R.id.menu_switchs_reset) {
			handlePauseMenu();
			pad_data = 0;
			pad_data |= IButtons.VALUE_RESET;
			SDLActivity.setPadData(0, pad_data);
			return true;
		} else if (itemId == R.id.menu_quit) {
			dialogConfirmExit();
			return true;
		}else if(itemId==android.R.id.home)
			{
			 handlePauseMenu();
			 return true;
			}else {
			return super.onOptionsItemSelected(item);
		}
	}

	public void selectEffect() {
		final CharSequence[] charseq = mEffectList.getCharSequenceList();
		new AlertDialog.Builder(activity).setTitle("Choose an effect")
				.setItems(charseq, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						Utility.log("Selected effect: "
								+ charseq[which].toString());
						mPrefs.setEffectFast(charseq[which].toString());
						applyRatioAndEffect();
					}
				}).create().show();
	}

	@SuppressWarnings("deprecation")
	public void applyRatioAndEffect() {
		float w = RelativeLayout.LayoutParams.MATCH_PARENT, h = RelativeLayout.LayoutParams.MATCH_PARENT;
		int orientation = getResources().getConfiguration().orientation;

		float width = display.getWidth();
		float height = display.getHeight();

		final float ratio = ((float) mScreenEmuSizeX / (float) mScreenEmuSizeY);
		Utility.log("Display: " + (int) width + "x" + (int) height + "(ratio:"
				+ ratio + ")");

		switch (EffectList.ScreenSize.values()[mPrefs.getScreenSize()]) {
		case EFFECT_FITSCREEN: {
			mMessage.showToastMessageShort("screen: fit");
			final float scaledw = (float) height * ratio;
			if (scaledw > width) {
				w = width;
				h = (float) w / ratio;
			} else {
				w = scaledw;
				h = display.getHeight();
			}
			break;
		}

		case EFFECT_43: {
			mMessage.showToastMessageShort("screen: 4:3");
			final float scaledw = (float) height * ((float) 4 / (float) 3);
			if (scaledw > width) {
				w = width;
				h = (float) w / ratio;
			} else {
				w = scaledw;
				h = display.getHeight();
			}
			break;
		}

		case EFFECT_FULLSCREEN:
			mMessage.showToastMessageShort("screen: full");
			break;

		case EFFECT_ORIGINALSCREEN:
			mMessage.showToastMessageShort("screen: original");
			w = mScreenEmuSizeX;
			h = mScreenEmuSizeY;
			break;

		case EFFECT_2X:
			mMessage.showToastMessageShort("screen: 2x");
			w = mScreenEmuSizeX * 2;
			h = mScreenEmuSizeY * 2;
			if (w > width) {
				mPrefs.setScreenSize(2);
				applyRatioAndEffect();
				return;
			}
			break;
		}

		Utility.log("View: " + (int) w + "x" + (int) h);
		RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(
				(int) w, (int) h);
		p.addRule(RelativeLayout.CENTER_HORIZONTAL);
		p.addRule(orientation == Configuration.ORIENTATION_PORTRAIT ? RelativeLayout.ALIGN_PARENT_TOP
				: RelativeLayout.CENTER_VERTICAL);
		mSurface.setLayoutParams(p);
		effectView
				.applyEffect(p, mEffectList.getByName(mPrefs.getEffectFast()));
		inputView.requestLayout();
	}

	public void dialogConfirmExit() {
		activity.runOnUiThread(new Runnable() {
			public void run() {
				new AlertDialog.Builder(activity)
						.setTitle("Confirm")
						.setMessage("\nStop emulation ?\n")
						.setPositiveButton("Confirm",
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog,
											int whichButton) {
										resume();
										activity.finish();
									}
								})
						.setNegativeButton("Cancel",
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog,
											int whichButton) {
									}
								}).create().show();
			}
		});
	}

	private final class InputEditActionMode implements ActionMode.Callback {
		boolean canceled = false;
		int buttons = -1;

		public InputEditActionMode(int bcount) {
			buttons = bcount;
		}

		@Override
		public boolean onCreateActionMode(ActionMode mode, Menu menu) {
			actionBarHeight = actionBar.getHeight();
			Utility.loge("actionBarHeight: " + actionBarHeight);
			hideNavBar();
			mMessage.showToastMessageLong("Touch buttons to move them...");
			inputView.setEditMode(true);
			SDLActivity.this.getMenuInflater().inflate(R.menu.menu_edit_input, menu);
			return true;
		}

		@Override
		public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
			return false;
		}

		@Override
		public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
			Utility.log("onActionItemClicked: " + item.getTitle() + " ("
					+ item.getItemId() + ")");

			float stick_scale = inputView.getStickScale();
			int stick_alpha = inputView.getStickAlpha();
			float buttons_scale = inputView.getButtonsScale();

			int id = item.getItemId();
			// switch ( item.getItemId() )
			{
				if (id == R.id.menu_edit_scale_sminus) {
					stick_scale -= 0.1f;
					inputView.setStickScale(stick_scale);
					return true;
				} else if (id == R.id.menu_edit_scale_splus) {
					stick_scale += 0.1f;
					inputView.setStickScale(stick_scale);
					return true;
				} else if (id == R.id.menu_edit_scale_bminus) {
					buttons_scale -= 0.1f;
					inputView.setButtonsScale(buttons_scale);
					return true;
				} else if (id == R.id.menu_edit_scale_bplus) {
					buttons_scale += 0.1f;
					inputView.setButtonsScale(buttons_scale);
					return true;
				} else if (id == R.id.menu_edit_alpha_minus) {
					stick_alpha -= 10;
					inputView.setAlpha(stick_alpha);
					return true;
				} else if (id == R.id.menu_edit_alpha_plus) {
					stick_alpha += 10;
					inputView.setAlpha(stick_alpha);
					return true;
				} else if (id == R.id.menu_edit_cancel) {
					canceled = true;
					mode.finish();
					return true;
				}
				return false;
			}
		}

		@Override
		public void onDestroyActionMode(ActionMode mode) {
			Utility.log("onDestroyActionMode");
			inputView.setEditMode(false);
			showNavBar();
			if (!canceled)
				inputView.save(buttons);
		}
	}

	// Controls
	public void setAnalogData(int i, float x, float y) {
	}


	@Override
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		if (this.inputHardwareEdit)
			return true;

		if (!inputView.isShown() && !actionBar.isShowing())
			return inputHardware.onKey(v, keyCode, event);

		return false;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (this.inputHardwareEdit)
			return true;

		switch (keyCode) {
		case KeyEvent.KEYCODE_SEARCH:
			return true;

		case KeyEvent.KEYCODE_BACK:
			return handlePauseMenu();

		case KeyEvent.KEYCODE_MENU:
			if (actionBar.isShowing())
				return super.onKeyDown(keyCode, event);
			else
				return handlePauseMenu();
		}
		return super.onKeyDown(keyCode, event);
	}

	public boolean handlePauseMenu() {
		Utility.log("### actionBar.isShowing: " + actionBar.isShowing());
		if (actionBar.isShowing()) {
			stateMenu.setVisibility(View.GONE);
			hideNavBar();
			resume();
		} else {
			showNavBar();
			pause();
		}
		return true;
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// ignore orientation/keyboard change
		super.onConfigurationChanged(newConfig);
		applyRatioAndEffect();
	}

	@Override
	protected void onPause() {
		Utility.log("onPause()");
		super.onPause();
		pause();
	}

	@Override
	protected void onResume() {
		Utility.log("onResume()");
		super.onResume();
		resume();
	}

	@Override
	protected void onDestroy() {
		Utility.log("onDestroy()");
		stop();
		super.onDestroy();

        if (SDLActivity.mBrokenLibraries) {
           super.onDestroy();
           // Reset everything in case the user re opens the app
           SDLActivity.initialize();
           return;
        }
        

        // Send a quit message to the application
        SDLActivity.mExitCalledFromJava = true;
        SDLActivity.nativeQuit();

        // Now wait for the SDL thread to quit
        if (SDLActivity.mSDLThread != null) {
            try {
            	SDLActivity.mSDLThread.join();
            } catch(Exception e) {
                Log.v(TAG, "Problem stopping thread: " + e);
            }
            SDLActivity.mSDLThread = null;

            //Log.v(TAG, "Finished waiting for SDL thread");
        }
        
        // Reset everything in case the user re opens the app
        SDLActivity.initialize();
	}

	
	private void showInputHardwareDialog() {
		inputHardwareEdit = true;
		inputHardwareButtonNow = 0;
		inputHardwareDialog = new AlertDialog.Builder(this)
				.setTitle("Map hardware keys")
				.setMessage(
						"\nPlease press a button for: "
								+ HardwareInput.ButtonKeys
										.get(inputHardwareButtonNow) + "\n")
				.create();

		inputHardwareDialog
				.setOnKeyListener(new DialogInterface.OnKeyListener() {
					@Override
					public boolean onKey(DialogInterface dialog, int keyCode,
							KeyEvent event) {
						if (event.getAction() == KeyEvent.ACTION_DOWN) {
							mPrefs.setPad(HardwareInput.ButtonKeys
									.get(inputHardwareButtonNow), keyCode);
							inputHardwareButtonNow++;
							if (inputHardwareButtonNow < HardwareInput.ButtonKeys
									.size()) {
								inputHardwareDialog.setMessage("\nPlease press a button for: "
										+ HardwareInput.ButtonKeys
												.get(inputHardwareButtonNow)
										+ "\n");
							} else {
								dialog.dismiss();
								inputHardwareEdit = false;
							}
						}
						return true;
					}
				});
		inputHardwareDialog.show();
	}

	public static void resume() {
		if (ispaused() == 0)
			return;

		resumeemu();
		if (SDLActivity.mBrokenLibraries) {
			return;
		}

		SDLActivity.handleResume();

		mAudioTrack.play();
	}

	/**
	 * Called by onResume or surfaceCreated. An actual resume should be done
	 * only when the surface is ready. Note: Some Android variants may send
	 * multiple surfaceChanged events, so we don't need to resume every time we
	 * get one of those events, only if it comes after surfaceDestroyed
	 */
	public static void handleResume() {
		if (SDLActivity.mIsPaused && SDLActivity.mIsSurfaceReady
				&& SDLActivity.mHasFocus) {
			SDLActivity.mIsPaused = false;
			SDLActivity.nativeResume();
			mSurface.handleResume();
		}
	}

	/* The native thread has finished */
	public static void handleNativeExit() {
		SDLActivity.mSDLThread = null;
		SDLActivity.activity.finish();
	}

	public static void pause() {
		if (ispaused() == 1)
			return;

		pauseemu();
		if (SDLActivity.mBrokenLibraries) {
			return;
		}

		SDLActivity.handlePause();
	}

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
      //  Log.v(TAG, "onWindowFocusChanged(): " + hasFocus);

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        SDLActivity.mHasFocus = hasFocus;
        if (hasFocus) {
            SDLActivity.handleResume();
        }
    }

    @Override
    public void onLowMemory() {
      //  Log.v(TAG, "onLowMemory()");
        super.onLowMemory();

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        SDLActivity.nativeLowMemory();
    }
    

	/**
	 * Called by onPause or surfaceDestroyed. Even if surfaceDestroyed is the
	 * first to be called, mIsSurfaceReady should still be set to 'true' during
	 * the call to onPause (in a usual scenario).
	 */
	public static void handlePause() {
		if (!SDLActivity.mIsPaused && SDLActivity.mIsSurfaceReady) {
			SDLActivity.mIsPaused = true;
			SDLActivity.nativePause();
			mSurface.handlePause();
		}
	}

	public static void stop() {
		if (ispaused() == 1)
			resume();

		emustop();

		System.gc();
	}
    // Messages from the SDLMain thread
    static final int COMMAND_CHANGE_TITLE = 1;
    static final int COMMAND_UNUSED = 2;
    static final int COMMAND_TEXTEDIT_HIDE = 3;
    static final int COMMAND_SET_KEEP_SCREEN_ON = 5;

    protected static final int COMMAND_USER = 0x8000;
    /**
     * This method is called by SDL if SDL did not handle a message itself.
     * This happens if a received message contains an unsupported command.
     * Method can be overwritten to handle Messages in a different class.
     * @param command the command of the message.
     * @param param the parameter of the message. May be null.
     * @return if the message was handled in overridden method.
     */
    protected boolean onUnhandledMessage(int command, Object param) {
        return false;
    }
    /**
     * This method is called by SDL using JNI.
     */
    public static Context getContext() {
        return activity;
    }
    /**
     * A Handler class for Messages from native SDL applications.
     * It uses current Activities as target (e.g. for the title).
     * static to prevent implicit references to enclosing object.
     */
    protected static class SDLCommandHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Context context = getContext();
            if (context == null) {
                Log.e(TAG, "error handling message, getContext() returned null");
                return;
            }
            switch (msg.arg1) {
            case COMMAND_CHANGE_TITLE:
                if (context instanceof Activity) {
                    ((Activity) context).setTitle((String)msg.obj);
                } else {
                    Log.e(TAG, "error handling message, getContext() returned no Activity");
                }
                break;
            case COMMAND_TEXTEDIT_HIDE:
                if (mTextEdit != null) {
                    // Note: On some devices setting view to GONE creates a flicker in landscape.
                    // Setting the View's sizes to 0 is similar to GONE but without the flicker.
                    // The sizes will be set to useful values when the keyboard is shown again.
                    mTextEdit.setLayoutParams(new RelativeLayout.LayoutParams(0, 0));

                    InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(mTextEdit.getWindowToken(), 0);
                }
                break;
            case COMMAND_SET_KEEP_SCREEN_ON:
            {
                Window window = ((Activity) context).getWindow();
                if (window != null) {
                    if ((msg.obj instanceof Integer) && (((Integer) msg.obj).intValue() != 0)) {
                        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    } else {
                        window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                    }
                }
                break;
            }
            default:
                if ((context instanceof SDLActivity) && !((SDLActivity) context).onUnhandledMessage(msg.arg1, msg.obj)) {
                    Log.e(TAG, "error handling message, command is " + msg.arg1);
                }
            }
        }
    }

    // Handler for the messages
    Handler commandHandler = new SDLCommandHandler();

    // Send a message from the SDLMain thread
    boolean sendCommand(int command, Object data) {
        Message msg = commandHandler.obtainMessage();
        msg.arg1 = command;
        msg.obj = data;
        return commandHandler.sendMessage(msg);
    }
    
    /**
     * This method is called by SDL using JNI.
     * @return result of getSystemService(name) but executed on UI thread.
     */
    public Object getSystemServiceFromUiThread(final String name) {
        final Object lock = new Object();
        final Object[] results = new Object[2]; // array for writable variables
        synchronized (lock) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    synchronized (lock) {
                        results[0] = getSystemService(name);
                        results[1] = Boolean.TRUE;
                        lock.notify();
                    }
                }
            });
            if (results[1] == null) {
                try {
                    lock.wait();
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
            }
        }
        return results[0];
    }

    static class ShowTextInputTask implements Runnable {
        /*
         * This is used to regulate the pan&scan method to have some offset from
         * the bottom edge of the input region and the top edge of an input
         * method (soft keyboard)
         */
        static final int HEIGHT_PADDING = 15;

        public int x, y, w, h;

        public ShowTextInputTask(int x, int y, int w, int h) {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;
        }

        @Override
        public void run() {
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(w, h + HEIGHT_PADDING);
            params.leftMargin = x;
            params.topMargin = y;

            if (mTextEdit == null) {
                mTextEdit = new DummyEdit(getContext());

                mLayout.addView(mTextEdit, params);
            } else {
                mTextEdit.setLayoutParams(params);
            }

            mTextEdit.setVisibility(View.VISIBLE);
            mTextEdit.requestFocus();

            InputMethodManager imm = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.showSoftInput(mTextEdit, 0);
        }
    }
 // Audio

    /**
     * This method is called by SDL using JNI.
     */
    public static int audioOpen(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        int channelConfig = isStereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);

        Log.v(TAG, "SDL audio: wanted " + (isStereo ? "stereo" : "mono") + " " + (is16Bit ? "16-bit" : "8-bit") + " " + (sampleRate / 1000f) + "kHz, " + desiredFrames + " frames buffer");

        // Let the user pick a larger buffer if they really want -- but ye
        // gods they probably shouldn't, the minimums are horrifyingly high
        // latency already
        desiredFrames = Math.max(desiredFrames, (AudioTrack.getMinBufferSize(sampleRate, channelConfig, audioFormat) + frameSize - 1) / frameSize);

        if (mAudioTrack == null) {
            mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate,
                    channelConfig, audioFormat, desiredFrames * frameSize, AudioTrack.MODE_STREAM);

            // Instantiating AudioTrack can "succeed" without an exception and the track may still be invalid
            // Ref: https://android.googlesource.com/platform/frameworks/base/+/refs/heads/master/media/java/android/media/AudioTrack.java
            // Ref: http://developer.android.com/reference/android/media/AudioTrack.html#getState()

            if (mAudioTrack.getState() != AudioTrack.STATE_INITIALIZED) {
                Log.e(TAG, "Failed during initialization of Audio Track");
                mAudioTrack = null;
                return -1;
            }

            mAudioTrack.play();
        }

        Log.v(TAG, "SDL audio: got " + ((mAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono") + " " + ((mAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit") + " " + (mAudioTrack.getSampleRate() / 1000f) + "kHz, " + desiredFrames + " frames buffer");

        return 0;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void audioWriteShortBuffer(short[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w(TAG, "SDL audio: error return from write(short)");
                return;
            }
        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void audioWriteByteBuffer(byte[] buffer) {
        for (int i = 0; i < buffer.length; ) {
            int result = mAudioTrack.write(buffer, i, buffer.length - i);
            if (result > 0) {
                i += result;
            } else if (result == 0) {
                try {
                    Thread.sleep(1);
                } catch(InterruptedException e) {
                    // Nom nom
                }
            } else {
                Log.w(TAG, "SDL audio: error return from write(byte)");
                return;
            }
        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static int captureOpen(int sampleRate, boolean is16Bit, boolean isStereo, int desiredFrames) {
        int channelConfig = isStereo ? AudioFormat.CHANNEL_CONFIGURATION_STEREO : AudioFormat.CHANNEL_CONFIGURATION_MONO;
        int audioFormat = is16Bit ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int frameSize = (isStereo ? 2 : 1) * (is16Bit ? 2 : 1);

        Log.v(TAG, "SDL capture: wanted " + (isStereo ? "stereo" : "mono") + " " + (is16Bit ? "16-bit" : "8-bit") + " " + (sampleRate / 1000f) + "kHz, " + desiredFrames + " frames buffer");

        // Let the user pick a larger buffer if they really want -- but ye
        // gods they probably shouldn't, the minimums are horrifyingly high
        // latency already
        desiredFrames = Math.max(desiredFrames, (AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat) + frameSize - 1) / frameSize);

        if (mAudioRecord == null) {
            mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.DEFAULT, sampleRate,
                    channelConfig, audioFormat, desiredFrames * frameSize);

            // see notes about AudioTrack state in audioOpen(), above. Probably also applies here.
            if (mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
                Log.e(TAG, "Failed during initialization of AudioRecord");
                mAudioRecord.release();
                mAudioRecord = null;
                return -1;
            }

            mAudioRecord.startRecording();
        }

        Log.v(TAG, "SDL capture: got " + ((mAudioRecord.getChannelCount() >= 2) ? "stereo" : "mono") + " " + ((mAudioRecord.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit") + " " + (mAudioRecord.getSampleRate() / 1000f) + "kHz, " + desiredFrames + " frames buffer");

        return 0;
    }

    /** This method is called by SDL using JNI. */
    public static int captureReadShortBuffer(short[] buffer, boolean blocking) {
        // !!! FIXME: this is available in API Level 23. Until then, we always block.  :(
        //return mAudioRecord.read(buffer, 0, buffer.length, blocking ? AudioRecord.READ_BLOCKING : AudioRecord.READ_NON_BLOCKING);
        return mAudioRecord.read(buffer, 0, buffer.length);
    }

    /** This method is called by SDL using JNI. */
    public static int captureReadByteBuffer(byte[] buffer, boolean blocking) {
        // !!! FIXME: this is available in API Level 23. Until then, we always block.  :(
        //return mAudioRecord.read(buffer, 0, buffer.length, blocking ? AudioRecord.READ_BLOCKING : AudioRecord.READ_NON_BLOCKING);
        return mAudioRecord.read(buffer, 0, buffer.length);
    }


    /** This method is called by SDL using JNI. */
    public static void audioClose() {
        if (mAudioTrack != null) {
            mAudioTrack.stop();
            mAudioTrack.release();
            mAudioTrack = null;
        }
    }

    /** This method is called by SDL using JNI. */
    public static void captureClose() {
        if (mAudioRecord != null) {
            mAudioRecord.stop();
            mAudioRecord.release();
            mAudioRecord = null;
        }
    }


    // Input

    /**
     * This method is called by SDL using JNI.
     * @return an array which may be empty but is never null.
     */
    public static int[] inputGetInputDeviceIds(int sources) {
        int[] ids = InputDevice.getDeviceIds();
        int[] filtered = new int[ids.length];
        int used = 0;
        for (int i = 0; i < ids.length; ++i) {
            InputDevice device = InputDevice.getDevice(ids[i]);
            if ((device != null) && ((device.getSources() & sources) != 0)) {
                filtered[used++] = device.getId();
            }
        }
        return Arrays.copyOf(filtered, used);
    }

    // Joystick glue code, just a series of stubs that redirect to the SDLJoystickHandler instance
    public static boolean handleJoystickMotionEvent(MotionEvent event) {
        return mJoystickHandler.handleMotionEvent(event);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void pollInputDevices() {
        if (SDLActivity.mSDLThread != null) {
            mJoystickHandler.pollInputDevices();
        }
    }

    // Check if a given device is considered a possible SDL joystick
    public static boolean isDeviceSDLJoystick(int deviceId) {
        InputDevice device = InputDevice.getDevice(deviceId);
        // We cannot use InputDevice.isVirtual before API 16, so let's accept
        // only nonnegative device ids (VIRTUAL_KEYBOARD equals -1)
        if ((device == null) || (deviceId < 0)) {
            return false;
        }
        int sources = device.getSources();
        return (((sources & InputDevice.SOURCE_CLASS_JOYSTICK) == InputDevice.SOURCE_CLASS_JOYSTICK) ||
                ((sources & InputDevice.SOURCE_DPAD) == InputDevice.SOURCE_DPAD) ||
                ((sources & InputDevice.SOURCE_GAMEPAD) == InputDevice.SOURCE_GAMEPAD)
        );
    }

    // APK expansion files support

    /** com.android.vending.expansion.zipfile.ZipResourceFile object or null. */
    private Object expansionFile;

    /** com.android.vending.expansion.zipfile.ZipResourceFile's getInputStream() or null. */
    private Method expansionFileMethod;

    /**
     * This method is called by SDL using JNI.
     * @return an InputStream on success or null if no expansion file was used.
     * @throws IOException on errors. Message is set for the SDL error message.
     */
    public InputStream openAPKExpansionInputStream(String fileName) throws IOException {
        // Get a ZipResourceFile representing a merger of both the main and patch files
        if (expansionFile == null) {
            String mainHint = nativeGetHint("SDL_ANDROID_APK_EXPANSION_MAIN_FILE_VERSION");
            if (mainHint == null) {
                return null; // no expansion use if no main version was set
            }
            String patchHint = nativeGetHint("SDL_ANDROID_APK_EXPANSION_PATCH_FILE_VERSION");
            if (patchHint == null) {
                return null; // no expansion use if no patch version was set
            }

            Integer mainVersion;
            Integer patchVersion;
            try {
                mainVersion = Integer.valueOf(mainHint);
                patchVersion = Integer.valueOf(patchHint);
            } catch (NumberFormatException ex) {
                ex.printStackTrace();
                throw new IOException("No valid file versions set for APK expansion files", ex);
            }

            try {
                // To avoid direct dependency on Google APK expansion library that is
                // not a part of Android SDK we access it using reflection
                expansionFile = Class.forName("com.android.vending.expansion.zipfile.APKExpansionSupport")
                    .getMethod("getAPKExpansionZipFile", Context.class, int.class, int.class)
                    .invoke(null, this, mainVersion, patchVersion);

                expansionFileMethod = expansionFile.getClass()
                    .getMethod("getInputStream", String.class);
            } catch (Exception ex) {
                ex.printStackTrace();
                expansionFile = null;
                expansionFileMethod = null;
                throw new IOException("Could not access APK expansion support library", ex);
            }
        }

        // Get an input stream for a known file inside the expansion file ZIPs
        InputStream fileStream;
        try {
            fileStream = (InputStream)expansionFileMethod.invoke(expansionFile, fileName);
        } catch (Exception ex) {
            // calling "getInputStream" failed
            ex.printStackTrace();
            throw new IOException("Could not open stream from APK expansion file", ex);
        }

        if (fileStream == null) {
            // calling "getInputStream" was successful but null was returned
            throw new IOException("Could not find path in APK expansion file");
        }

        return fileStream;
    }

    // Messagebox

    /** Result of current messagebox. Also used for blocking the calling thread. */
    protected final int[] messageboxSelection = new int[1];

    /** Id of current dialog. */
    protected int dialogs = 0;

    /**
     * This method is called by SDL using JNI.
     * Shows the messagebox from UI thread and block calling thread.
     * buttonFlags, buttonIds and buttonTexts must have same length.
     * @param buttonFlags array containing flags for every button.
     * @param buttonIds array containing id for every button.
     * @param buttonTexts array containing text for every button.
     * @param colors null for default or array of length 5 containing colors.
     * @return button id or -1.
     */
    public int messageboxShowMessageBox(
            final int flags,
            final String title,
            final String message,
            final int[] buttonFlags,
            final int[] buttonIds,
            final String[] buttonTexts,
            final int[] colors) {

        messageboxSelection[0] = -1;

        // sanity checks

        if ((buttonFlags.length != buttonIds.length) && (buttonIds.length != buttonTexts.length)) {
            return -1; // implementation broken
        }

        // collect arguments for Dialog

        final Bundle args = new Bundle();
        args.putInt("flags", flags);
        args.putString("title", title);
        args.putString("message", message);
        args.putIntArray("buttonFlags", buttonFlags);
        args.putIntArray("buttonIds", buttonIds);
        args.putStringArray("buttonTexts", buttonTexts);
        args.putIntArray("colors", colors);

        // trigger Dialog creation on UI thread

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                showDialog(dialogs++, args);
            }
        });

        // block the calling thread

        synchronized (messageboxSelection) {
            try {
                messageboxSelection.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
                return -1;
            }
        }

        // return selected value

        return messageboxSelection[0];
    }

    @Override
    protected Dialog onCreateDialog(int ignore, Bundle args) {

        // TODO set values from "flags" to messagebox dialog

        // get colors

        int[] colors = args.getIntArray("colors");
        int backgroundColor;
        int textColor;
        int buttonBorderColor;
        int buttonBackgroundColor;
        int buttonSelectedColor;
        if (colors != null) {
            int i = -1;
            backgroundColor = colors[++i];
            textColor = colors[++i];
            buttonBorderColor = colors[++i];
            buttonBackgroundColor = colors[++i];
            buttonSelectedColor = colors[++i];
        } else {
            backgroundColor = Color.TRANSPARENT;
            textColor = Color.TRANSPARENT;
            buttonBorderColor = Color.TRANSPARENT;
            buttonBackgroundColor = Color.TRANSPARENT;
            buttonSelectedColor = Color.TRANSPARENT;
        }

        // create dialog with title and a listener to wake up calling thread

        final Dialog dialog = new Dialog(this);
        dialog.setTitle(args.getString("title"));
        dialog.setCancelable(false);
        dialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface unused) {
                synchronized (messageboxSelection) {
                    messageboxSelection.notify();
                }
            }
        });

        // create text

        TextView message = new TextView(this);
        message.setGravity(Gravity.CENTER);
        message.setText(args.getString("message"));
        if (textColor != Color.TRANSPARENT) {
            message.setTextColor(textColor);
        }

        // create buttons

        int[] buttonFlags = args.getIntArray("buttonFlags");
        int[] buttonIds = args.getIntArray("buttonIds");
        String[] buttonTexts = args.getStringArray("buttonTexts");

        final SparseArray<Button> mapping = new SparseArray<Button>();

        LinearLayout buttons = new LinearLayout(this);
        buttons.setOrientation(LinearLayout.HORIZONTAL);
        buttons.setGravity(Gravity.CENTER);
        for (int i = 0; i < buttonTexts.length; ++i) {
            Button button = new Button(this);
            final int id = buttonIds[i];
            button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    messageboxSelection[0] = id;
                    dialog.dismiss();
                }
            });
            if (buttonFlags[i] != 0) {
                // see SDL_messagebox.h
                if ((buttonFlags[i] & 0x00000001) != 0) {
                    mapping.put(KeyEvent.KEYCODE_ENTER, button);
                }
                if ((buttonFlags[i] & 0x00000002) != 0) {
                    mapping.put(111, button); /* API 11: KeyEvent.KEYCODE_ESCAPE */
                }
            }
            button.setText(buttonTexts[i]);
            if (textColor != Color.TRANSPARENT) {
                button.setTextColor(textColor);
            }
            if (buttonBorderColor != Color.TRANSPARENT) {
                // TODO set color for border of messagebox button
            }
            if (buttonBackgroundColor != Color.TRANSPARENT) {
                Drawable drawable = button.getBackground();
                if (drawable == null) {
                    // setting the color this way removes the style
                    button.setBackgroundColor(buttonBackgroundColor);
                } else {
                    // setting the color this way keeps the style (gradient, padding, etc.)
                    drawable.setColorFilter(buttonBackgroundColor, PorterDuff.Mode.MULTIPLY);
                }
            }
            if (buttonSelectedColor != Color.TRANSPARENT) {
                // TODO set color for selected messagebox button
            }
            buttons.addView(button);
        }

        // create content

        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        content.addView(message);
        content.addView(buttons);
        if (backgroundColor != Color.TRANSPARENT) {
            content.setBackgroundColor(backgroundColor);
        }

        // add content to dialog and return

        dialog.setContentView(content);
        dialog.setOnKeyListener(new Dialog.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface d, int keyCode, KeyEvent event) {
                Button button = mapping.get(keyCode);
                if (button != null) {
                    if (event.getAction() == KeyEvent.ACTION_UP) {
                        button.performClick();
                    }
                    return true; // also for ignored actions
                }
                return false;
            }
        });

        return dialog;
    }


    /**
     * This method is called by SDL using JNI.
     */
    public static boolean showTextInput(int x, int y, int w, int h) {
        // Transfer the task to the main thread as a Runnable
        return activity.commandHandler.post(new ShowTextInputTask(x, y, w, h));
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static Surface getNativeSurface() {
        return SDLActivity.mSurface.getNativeSurface();
    }

    
    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {

        if (SDLActivity.mBrokenLibraries) {
           return false;
        }

        int keyCode = event.getKeyCode();
        // Ignore certain special keys so they're handled by Android
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == 168 || /* API 11: KeyEvent.KEYCODE_ZOOM_IN */
            keyCode == 169 /* API 11: KeyEvent.KEYCODE_ZOOM_OUT */
            ) {
            return false;
        }
        return super.dispatchKeyEvent(event);
    }
	// private static String mErrorMessage = null;
	private static int progress = 0;
	private static int max = 0;
	private static String progressMessage = "Please Wait";
	private static Handler mHandler = new Handler();
	private static Runnable updateProgressBar = new Runnable() {
		@Override
		public void run() {
			// mMessage.setDialogProgress( progressMessage, progress );
			mMessage.show(progressMessage, progress, max);
			mHandler.postDelayed(updateProgressBar, 500);
		}
	};

	public static void setErrorMessage(String pMessage) {
		Utility.loge("###setErrorMessage### ==> " + pMessage);
		Intent i = activity.getIntent();
		i.putExtra("error", pMessage);
		activity.setResult(RESULT_OK, i);
		activity.finish();
	}

	public static void showProgressBar(String pMessage, int pMax) {
		// Utility.loge( "###showProgressBar### ==> " + pMessage );
		max = pMax;
		mMessage.show(pMessage, 0, pMax);
		mHandler.post(updateProgressBar);
	}

	public static void hideProgressBar() {
		// Utility.loge( "###hideProgressBar###" );
		mHandler.removeCallbacks(updateProgressBar);
		mMessage.hide();
	}

	public static void setProgressBar(String pMessage, int pProgress) {
		// Utility.loge( "###setProgressBar### ==> " + pProgress );
		// progressMessage = pMessage;
		// progress = pProgress;
		mMessage.show(pMessage);
	}

}


/* This is a fake invisible editor view that receives the input and defines the
 * pan&scan region
 */
class DummyEdit extends View implements View.OnKeyListener {
    InputConnection ic;

    public DummyEdit(Context context) {
        super(context);
        setFocusableInTouchMode(true);
        setFocusable(true);
        setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {

        // This handles the hardware keyboard input
        if (event.isPrintingKey() || keyCode == KeyEvent.KEYCODE_SPACE) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                ic.commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            return true;
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }

        return false;
    }

    //
    @Override
    public boolean onKeyPreIme (int keyCode, KeyEvent event) {
        // As seen on StackOverflow: http://stackoverflow.com/questions/7634346/keyboard-hide-event
        // FIXME: Discussion at http://bugzilla.libsdl.org/show_bug.cgi?id=1639
        // FIXME: This is not a 100% effective solution to the problem of detecting if the keyboard is showing or not
        // FIXME: A more effective solution would be to assume our Layout to be RelativeLayout or LinearLayout
        // FIXME: And determine the keyboard presence doing this: http://stackoverflow.com/questions/2150078/how-to-check-visibility-of-software-keyboard-in-android
        // FIXME: An even more effective way would be if Android provided this out of the box, but where would the fun be in that :)
        if (event.getAction()==KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
            if (SDLActivity.mTextEdit != null && SDLActivity.mTextEdit.getVisibility() == View.VISIBLE) {
                SDLActivity.onNativeKeyboardFocusLost();
            }
        }
        return super.onKeyPreIme(keyCode, event);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        ic = new SDLInputConnection(this, true);

        outAttrs.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD;
        outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI
                | 33554432 /* API 11: EditorInfo.IME_FLAG_NO_FULLSCREEN */;

        return ic;
    }
}

class SDLInputConnection extends BaseInputConnection {

    public SDLInputConnection(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);

    }

    @Override
    public boolean sendKeyEvent(KeyEvent event) {

        /*
         * This handles the keycodes from soft keyboard (and IME-translated
         * input from hardkeyboard)
         */
        int keyCode = event.getKeyCode();
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.isPrintingKey() || keyCode == KeyEvent.KEYCODE_SPACE) {
                commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            SDLActivity.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {

            SDLActivity.onNativeKeyUp(keyCode);
            return true;
        }
        return super.sendKeyEvent(event);
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {

        nativeCommitText(text.toString(), newCursorPosition);

        return super.commitText(text, newCursorPosition);
    }

    @Override
    public boolean setComposingText(CharSequence text, int newCursorPosition) {

        nativeSetComposingText(text.toString(), newCursorPosition);

        return super.setComposingText(text, newCursorPosition);
    }

    public native void nativeCommitText(String text, int newCursorPosition);

    public native void nativeSetComposingText(String text, int newCursorPosition);

    @Override
    public boolean deleteSurroundingText(int beforeLength, int afterLength) {
        // Workaround to capture backspace key. Ref: http://stackoverflow.com/questions/14560344/android-backspace-in-webview-baseinputconnection
        if (beforeLength == 1 && afterLength == 0) {
            // backspace
            return super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DEL))
                && super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_DEL));
        }

        return super.deleteSurroundingText(beforeLength, afterLength);
    }
}

/*
 * A null joystick handler for API level < 12 devices (the accelerometer is
 * handled separately)
 */
class SDLJoystickHandler {

	/**
	 * Handles given MotionEvent.
	 * 
	 * @param event
	 *            the event to be handled.
	 * @return if given event was processed.
	 */
	public boolean handleMotionEvent(MotionEvent event) {
		return false;
	}

	/**
	 * Handles adding and removing of input devices.
	 */
	public void pollInputDevices() {
	}
}

/* Actual joystick functionality available for API >= 12 devices */
class SDLJoystickHandler_API12 extends SDLJoystickHandler {

	static class SDLJoystick {
		public int device_id;
		public String name;
		public ArrayList<InputDevice.MotionRange> axes;
		public ArrayList<InputDevice.MotionRange> hats;
	}

	static class RangeComparator implements Comparator<InputDevice.MotionRange> {
		@Override
		public int compare(InputDevice.MotionRange arg0,
				InputDevice.MotionRange arg1) {
			return arg0.getAxis() - arg1.getAxis();
		}
	}

	private ArrayList<SDLJoystick> mJoysticks;

	public SDLJoystickHandler_API12() {

		mJoysticks = new ArrayList<SDLJoystick>();
	}

	@Override
	public void pollInputDevices() {
		int[] deviceIds = InputDevice.getDeviceIds();
		// It helps processing the device ids in reverse order
		// For example, in the case of the XBox 360 wireless dongle,
		// so the first controller seen by SDL matches what the receiver
		// considers to be the first controller

		for (int i = deviceIds.length - 1; i > -1; i--) {
			SDLJoystick joystick = getJoystick(deviceIds[i]);
			if (joystick == null) {
				joystick = new SDLJoystick();
				InputDevice joystickDevice = InputDevice
						.getDevice(deviceIds[i]);
				if (SDLActivity.isDeviceSDLJoystick(deviceIds[i])) {
					joystick.device_id = deviceIds[i];
					joystick.name = joystickDevice.getName();
					joystick.axes = new ArrayList<InputDevice.MotionRange>();
					joystick.hats = new ArrayList<InputDevice.MotionRange>();

					List<InputDevice.MotionRange> ranges = joystickDevice
							.getMotionRanges();
					Collections.sort(ranges, new RangeComparator());
					for (InputDevice.MotionRange range : ranges) {
						if ((range.getSource() & InputDevice.SOURCE_CLASS_JOYSTICK) != 0) {
							if (range.getAxis() == MotionEvent.AXIS_HAT_X
									|| range.getAxis() == MotionEvent.AXIS_HAT_Y) {
								joystick.hats.add(range);
							} else {
								joystick.axes.add(range);
							}
						}
					}

					mJoysticks.add(joystick);
					SDLActivity.nativeAddJoystick(joystick.device_id,
							joystick.name, 0, -1, joystick.axes.size(),
							joystick.hats.size() / 2, 0);
				}
			}
		}

		/* Check removed devices */
		ArrayList<Integer> removedDevices = new ArrayList<Integer>();
		for (int i = 0; i < mJoysticks.size(); i++) {
			int device_id = mJoysticks.get(i).device_id;
			int j;
			for (j = 0; j < deviceIds.length; j++) {
				if (device_id == deviceIds[j])
					break;
			}
			if (j == deviceIds.length) {
				removedDevices.add(Integer.valueOf(device_id));
			}
		}

		for (int i = 0; i < removedDevices.size(); i++) {
			int device_id = removedDevices.get(i).intValue();
			SDLActivity.nativeRemoveJoystick(device_id);
			for (int j = 0; j < mJoysticks.size(); j++) {
				if (mJoysticks.get(j).device_id == device_id) {
					mJoysticks.remove(j);
					break;
				}
			}
		}
	}

	protected SDLJoystick getJoystick(int device_id) {
		for (int i = 0; i < mJoysticks.size(); i++) {
			if (mJoysticks.get(i).device_id == device_id) {
				return mJoysticks.get(i);
			}
		}
		return null;
	}

	@Override
	public boolean handleMotionEvent(MotionEvent event) {
		if ((event.getSource() & InputDevice.SOURCE_JOYSTICK) != 0) {
			int actionPointerIndex = event.getActionIndex();
			int action = event.getActionMasked();
			switch (action) {
			case MotionEvent.ACTION_MOVE:
				SDLJoystick joystick = getJoystick(event.getDeviceId());
				if (joystick != null) {
					for (int i = 0; i < joystick.axes.size(); i++) {
						InputDevice.MotionRange range = joystick.axes.get(i);
						/* Normalize the value to -1...1 */
						float value = (event.getAxisValue(range.getAxis(),
								actionPointerIndex) - range.getMin())
								/ range.getRange() * 2.0f - 1.0f;
						SDLActivity.onNativeJoy(joystick.device_id, i, value);
					}
					for (int i = 0; i < joystick.hats.size(); i += 2) {
						int hatX = Math.round(event.getAxisValue(joystick.hats
								.get(i).getAxis(), actionPointerIndex));
						int hatY = Math.round(event.getAxisValue(joystick.hats
								.get(i + 1).getAxis(), actionPointerIndex));
						SDLActivity.onNativeHat(joystick.device_id, i / 2,
								hatX, hatY);
					}
				}
				break;
			default:
				break;
			}
		}
		return true;
	}
}
