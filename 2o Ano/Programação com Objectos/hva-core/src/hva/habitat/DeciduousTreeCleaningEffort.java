package hva.habitat;
public class DeciduousTreeCleaningEffort implements CleaningEffort {

    @Override
    public int getEffort(Season season) {
        return switch (season) {
            case WINTER -> 0;
            case SPRING -> 1;
            case SUMMER -> 2;
            case AUTUMN -> 5;
        };
    }
}
