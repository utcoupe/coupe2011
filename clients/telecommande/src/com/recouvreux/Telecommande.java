package com.recouvreux;


import android.app.Activity;
import android.app.Dialog;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
 

public class Telecommande extends Activity
{
	float WIDTH, HEIGHT;
	String SERVERIP;
	int SERVERPORT;
	float xscale, yscale;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
	    setContentView(R.layout.main);
	    
	    // on récupère la taille de l'écran
	    Display display = getWindowManager().getDefaultDisplay(); 
	    WIDTH = display.getWidth();
	    HEIGHT = display.getHeight();
	    xscale = (float) (3000.0 / WIDTH);
	    yscale = (float) (2100.0 / HEIGHT);
	    Log.d("UDP", ""+WIDTH+" "+HEIGHT);

		Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.parameters);
        dialog.setTitle("Server parameters :");
        Button buttonOK = (Button) dialog.findViewById(R.id.ok);
        buttonOK.setOnClickListener(new OKListener(dialog));
        dialog.show();
        
	    /*
	    // Kickoff the Server, it will
		// be 'listening' for one client packet
		new Thread(new Server()).start();
		// Give the Server some time for startup
		try {
		                Thread.sleep(500);
		        } catch (InterruptedException e) { }
		*/
		// Kickoff the Client
	    // new Thread(new Client("Hello from client")).start();
	}
	
	public boolean onTouchEvent(MotionEvent event)
	{
		int action = event.getAction();
		if (action == MotionEvent.ACTION_DOWN)
		{
			float x = event.getX();
			float y = event.getY();
			Log.d("UDP", "touch !"+x+" "+y);
		    new Thread(new UDPClient(SERVERIP, SERVERPORT, ""+(int) (x*xscale) +" "+ (int) (y*yscale))).start();
		}
		return true;
	}
	
	protected class OKListener implements OnClickListener
	{	 
        private Dialog dialog;
        public OKListener(Dialog dialog)
        {
                this.dialog = dialog;
        }

        public void onClick(View v)
        {
        	EditText serverip = (EditText) dialog.findViewById(R.id.serverip);
        	TextView serverport = (EditText) dialog.findViewById(R.id.serverport);
        	SERVERIP = serverip.getText().toString();
        	SERVERPORT = Integer.parseInt(serverport.getText().toString());
    		dialog.dismiss();
            Log.d("UDP",SERVERIP+" "+SERVERPORT);
        }
	}
}