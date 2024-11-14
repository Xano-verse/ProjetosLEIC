package hva.habitat;

public class EvergreenTreeCleaningEffort implements CleaningEffort {

    @Override
    public int getEffort(Season season) {
        return switch (season) {
            case WINTER -> 2;
            case SPRING -> 1;
            case SUMMER -> 1;
            case AUTUMN -> 1;
        };
    }
}
