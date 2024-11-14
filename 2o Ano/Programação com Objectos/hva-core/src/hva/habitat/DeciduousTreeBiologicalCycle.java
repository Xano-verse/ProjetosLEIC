package hva.habitat;

public class DeciduousTreeBiologicalCycle implements BiologicalCycle {

    @Override
    public String getCycle(Season season) {
        return switch (season) {
            case WINTER -> "SEMFOLHAS";
            case SPRING -> "GERARFOLHAS";
            case SUMMER -> "COMFOLHAS";
            case AUTUMN -> "LARGARFOLHAS";
        };
    }

}
