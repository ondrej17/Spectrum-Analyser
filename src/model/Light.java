package model;

import java.io.Serializable;

public class Light implements Serializable {

    private String name;
    private Boolean participation;


    public Light(String name, Boolean participation) {

        this.name = name;
        this.participation = participation;
    }

    // getters and setters

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Boolean getParticipation() {
        return participation;
    }

    public void setParticipation(Boolean participation) {
        this.participation = participation;
    }

}
