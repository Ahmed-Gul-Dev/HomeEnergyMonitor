package com.example.energymonitorexpo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainPower extends AppCompatActivity {
    TextView voltview,powerview,ampsview,volt3view,pfview,freqview,mainread;
    FirebaseDatabase db = FirebaseDatabase.getInstance();
    DatabaseReference node;
    DatabaseReference voltnode, powernode,ampsnode,volt3node,pfnode,freqnode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_power);

        mainread = (TextView) findViewById(R.id.tagline);
        voltview = (TextView) findViewById(R.id.volts);
        powerview = (TextView) findViewById(R.id.power);
        ampsview = (TextView) findViewById(R.id.amps);
        volt3view = (TextView) findViewById(R.id.volt3);
        pfview = (TextView) findViewById(R.id.pf);
        freqview = (TextView) findViewById(R.id.freq);

        node = db.getReference("Parameters");
        voltnode = node.child("voltage");
        powernode = node.child("pow1");
        ampsnode = node.child("amps1");
        volt3node = node.child("voltage3");
        pfnode = node.child("pf");
        freqnode = node.child("freq");

        voltnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                voltview.setText(value + " V");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        powernode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                powerview.setText(value + " kW");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        ampsnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                ampsview.setText(value + " A");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        volt3node.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                volt3view.setText(value + " V");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        pfnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                pfview.setText(value);
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        freqnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                freqview.setText(value + " Hz");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(MainPower.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });


        DatabaseReference connectedRef = FirebaseDatabase.getInstance().getReference(".info/connected");
        connectedRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                boolean connected = snapshot.getValue(Boolean.class);
                if (connected) {
                    mainread.setText("INTERNET CONNECTED ");
                } else {
                    mainread.setText("INTERNET CONNECTION FAILED");
                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

    public void onBackPressed() {
        super.onBackPressed();
        finish();
        startActivity(new Intent(MainPower.this, MainActivity.class));
    }
}