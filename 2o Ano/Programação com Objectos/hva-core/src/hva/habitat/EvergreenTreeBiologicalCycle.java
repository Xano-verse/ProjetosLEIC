package hva.habitat;
public class EvergreenTreeBiologicalCycle implements BiologicalCycle {

    @Override
    public String getCycle(Season season) {
        return switch (season) {
            case WINTER -> "LARGARFOLHAS";
            case SPRING -> "GERARFOLHAS";
            case SUMMER -> "COMFOLHAS";
            case AUTUMN -> "COMFOLHAS";
        };
    }
}
