package hva.habitat;

public enum Season {
    WINTER(3),
    SPRING(0),
    SUMMER(1),
    AUTUMN(2);

    private int seasonIndex;

    Season(int index) {
        this.seasonIndex = index;
    }

    public int getSeasonIndex() {
        return seasonIndex;
    }
}
