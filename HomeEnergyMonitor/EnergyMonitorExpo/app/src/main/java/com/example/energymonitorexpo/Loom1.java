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

public class Loom1 extends AppCompatActivity {
    TextView voltview,powerview,ampsview,speedview,frview,freqview,mainread;
    FirebaseDatabase db = FirebaseDatabase.getInstance();
    DatabaseReference node;
    DatabaseReference voltnode, powernode,ampsnode,speednode,frnode,freqnode;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_loom1);

        mainread = (TextView) findViewById(R.id.tagline);
        voltview = (TextView) findViewById(R.id.volts);
        powerview = (TextView) findViewById(R.id.power);
        ampsview = (TextView) findViewById(R.id.amps);
        speedview = (TextView) findViewById(R.id.speed);
        frview = (TextView) findViewById(R.id.flowrate);
        freqview = (TextView) findViewById(R.id.freq);

        node = db.getReference("Parameters");
        voltnode = node.child("voltage3");
        powernode = node.child("pow3");
        ampsnode = node.child("amps3");
        speednode = node.child("Sped");
        frnode = node.child("flowrate");
        freqnode = node.child("freq");

        voltnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                voltview.setText(value + " V");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
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
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
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
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        speednode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                speedview.setText(value + " Rpm");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
            }
        });
        frnode.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                String value = dataSnapshot.getValue(String.class); //load status
                frview.setText(value + " L/min");
            }
            @Override
            public void onCancelled(DatabaseError error) {
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
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
                Toast.makeText(Loom1.this, "Connection Error", Toast.LENGTH_LONG).show();
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
        startActivity(new Intent(Loom1.this, MainActivity.class));
    }
}