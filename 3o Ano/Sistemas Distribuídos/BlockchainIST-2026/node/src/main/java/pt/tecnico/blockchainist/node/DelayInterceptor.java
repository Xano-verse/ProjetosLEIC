package pt.tecnico.blockchainist.node;

import io.grpc.Metadata;
import io.grpc.ServerCall;
import io.grpc.ServerCallHandler;
import io.grpc.ServerInterceptor;

/**
 * gRPC server interceptor that introduces an artificial delay in request handling.
 *
 * This interceptor checks for a "delay" header in the incoming request metadata.
 * If present, it pauses the current thread for the specified number of milliseconds
 * before allowing the request to proceed. This can be useful for testing latency,
 * fault tolerance, or timing-related behaviors in distributed systems.
 */
public class DelayInterceptor implements ServerInterceptor {

    /**
     * Metadata key used to retrieve the delay value from request headers.
     */
    private static final Metadata.Key<String> DELAY_KEY =
            Metadata.Key.of("delay", Metadata.ASCII_STRING_MARSHALLER);

    /**
     * Intercepts incoming gRPC calls and applies a delay if specified in the headers.
     *
     * @param call the server call object
     * @param headers the metadata headers from the client request
     * @param next the handler to pass the call to the next interceptor or service
     * @param <ReqT> the type of the request
     * @param <RespT> the type of the response
     * @return a listener for handling incoming request messages
     */
    @Override
    public <ReqT, RespT> ServerCall.Listener<ReqT> interceptCall(
            ServerCall<ReqT, RespT> call,
            Metadata headers,
            ServerCallHandler<ReqT, RespT> next) {

        String delayHeader = headers.get(DELAY_KEY);

        if (delayHeader != null) {
            try {
                int delay = Integer.parseInt(delayHeader);
                Thread.sleep(delay);
            } catch (NumberFormatException e) {
                System.err.println("Invalid delay header: " + e.getMessage());
            } catch (InterruptedException e) {
                System.err.println("Thread was interrupted: " + e.getMessage());
            }
        }

        return next.startCall(call, headers);
    }
}