package com.example.testapp;

import androidx.annotation.NonNull;
import android.widget.Toast;

/**
 * @author drakeet
 */
public interface BadTokenListener {

    void onBadTokenCaught(@NonNull Toast toast);
}
