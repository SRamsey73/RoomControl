package c.setheramsey.roomcontrolandroid;

import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;

public class LevelAdjustment {

    ImageView[] bar = new ImageView[11];

    LevelAdjustment(ImageView[] adjusters) {

        for(int i = 0; i < 11; i++) {
            bar[i] = adjusters[i];        
        }
        bar[1].setOnTouchListener(bar1);
        bar[2].setOnTouchListener(bar2);
        bar[3].setOnTouchListener(bar3);
        bar[4].setOnTouchListener(bar4);
        bar[5].setOnTouchListener(bar5);
        bar[6].setOnTouchListener(bar6);
        bar[7].setOnTouchListener(bar7);
        bar[8].setOnTouchListener(bar8);
        bar[9].setOnTouchListener(bar9);
        bar[10].setOnTouchListener(bar10);
    }

    View.OnTouchListener bar1 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            bar[1].setImageResource(R.drawable.ic_light_bar_full);
            for(int i = 2; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar2 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 2; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 3; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar3 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 3; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 4; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar4 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 4; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 5; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar5 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 5; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 6; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar6 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 6; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 7; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar7 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 7; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 8; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar8 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 8; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 9; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar9 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 9; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            for(int i = 10; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_empty);
            }
            return true;
        }
    };

    View.OnTouchListener bar10 = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            for(int i = 1; i <= 10; i++) {
                bar[i].setImageResource(R.drawable.ic_light_bar_full);
            }
            return true;
        }
    };
}
